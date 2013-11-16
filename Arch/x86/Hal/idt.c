#include "idt.h"
#include <string.h>
#include <Hal.h>

#include "../Kernel/KernelDisplay.h"

#define __packed
struct idtr{

	uint16_t limit; //Size of the IDT
	uint32_t base;  //Start address of the IDT

}__attribute((packed));
#undef __packed


static struct idt_descriptor _idt[I86_MAX_INTERRUPTS]; // x86 have 256 Interrupt

static struct idtr _idtr; // This help us to define the cpu idtr register

static void idt_install(); // installs _idtr in idtr cpu reg

static void i86_default_handler();

static void idt_install(){
	 asm ("lidt %0": :"m"(_idtr));
} 

// Default handler, this will be used to cach unhandled sys interrupts
static void i86_default_handler(){

	kernelClrScr(0x18);
	kernelGotoXY(0,0);
	kernelSetColor(0x1e);
	kernelPuts("Default Handler: Unhandled Exception!");

	while(1);

}


// This function return the interrupt descriptor
struct idt_descriptor* i86_get_ir(uint32_t i){
	if(i>I86_MAX_INTERRUPTS)
		return 0;
	
	return &_idt[i];
}

// This function install a new interrupt handler (REMEMBER THAT IS POSSIBLE TO INSTALL UP TO 256 INTERRUPTS!!!)
int i86_install_ir(uint32_t i,uint16_t flags, uint16_t sel,I86_IRQ_HANDLER irq){

	if(i>I86_MAX_INTERRUPTS)
		return 0;
	if(!irq)
		return 0;

	uint32_t uiBase = (uint32_t) & (*irq); // Get Base Address of Interrupt Handler


	// Store base address into idt
	_idt[i].baseLo = (uint16_t)(uiBase & 0xffff);
	_idt[i].baseHi = (uint16_t)((uiBase >> 16) & 0xffff);
	_idt[i].reserved = 0;
	_idt[i].flags = (uint8_t)(flags);
	_idt[i].sel = sel;

	return 0;
}

// Initialize our IDT
int i86_idt_initialize(uint16_t codeSel){
	//Set up IDTR for CPU
	_idtr.limit = sizeof(struct idt_descriptor) * I86_MAX_INTERRUPTS -1 ;
	_idtr.base = (uint32_t)&_idt[0];
	// null out the idt
	memset((void*)&_idt[0],0,sizeof(struct idt_descriptor) * I86_MAX_INTERRUPTS -1);

	// register default handlers
	int i = 0;	
	for(i=0;i<I86_MAX_INTERRUPTS;i++)
		i86_install_ir(i,I86_IDT_DESC_PRESENT|I86_IDT_DESC_BIT32,codeSel,(I86_IRQ_HANDLER)i86_default_handler);

	// install the IDT
	idt_install();

	return 0;
}



