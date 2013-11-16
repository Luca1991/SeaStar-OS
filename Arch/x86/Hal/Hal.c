#include <Hal.h>
#include "cpu.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"


// Init Hardware
int  hal_initialize(){

	i86_cpu_initialize(); // Init x86 CPU
	i86_pic_initialize (0x20,0x28);
	i86_pit_initialize ();
	
	i86_pit_start_counter (100,I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);

	//! enable hardware interrupts
	enable_interrupt(); // This function is BUGGED!! We need to rewrite hw interrupts in ASM to avoid stack errors!
	
	return 0;
}

// Shutdown Hardware
int  hal_shoutdown(){

	i86_cpu_shutdown(); // Shutdown x86 CPU
	return 0;

}

// Generate Interrupt 
void  geninterrupt (int x) {
  __asm__("  movb %[x], %%al \n"
          "  movb %%al, genint+1 \n"
          "  jmp genint \n"
          "genint: \n"
          "  int $0 \n"
         : /* no outputs */
         : [x] "m" (x) /* use x as input */
         : "al" /* clobbers %al */
         );
}

// Comunicate to the hal that the interrupt is done
inline void interruptdone(unsigned int intno){

	if(intno>16)     // if interrupt number is greater than 16 then it is not a valid hardware interrupt
		return;
	
	if(intno>=8)	// if interrupt number is greater or equal than 8, we have to send the EOI (end of interrupt) to the second pic
		i86_pic_send_command(I86_PIC_OCW2_MASK_EOI,1);

	i86_pic_send_command(I86_PIC_OCW2_MASK_EOI,0); // always send the EOI to the primary pic

}

// Play a sound using the speaker
void sound (unsigned frequency){
	// 0x61 is the speaker addr. Frequency = 0 disable the speaker
	outportb(0x61,3|(unsigned char)(frequency<<2));
}


// Read a byte from portid
unsigned char inportb(unsigned short portid){ 
	unsigned char ret;
	asm volatile ("inb %1,%0":"=a"(ret):"Nd"(portid));
  	return ret;
}

// Write a byte (value) to portid
void outportb (unsigned short portid, unsigned char value){
	asm volatile( "outb %0, %1" : : "a"(value), "Nd"(portid) );
	
}

// Enable Hardware interrupts
void enable_interrupt(){
	asm("sti");
}

// Disable Hardware interrupts
void disable_interrupt(){
	asm("cli");
}


// Sets new interrupt vector
void setvect(int intno, void (far *vect)()){
	// install interrupt handler (ATTENTION: this will overwrite previous interrupt descriptor)
	i86_install_ir(intno,I86_IDT_DESC_PRESENT|I86_IDT_DESC_BIT32,0x8,vect);

}

// Returns current interrupt vector
void (far * getvect(int intno))(){

	struct idt_descriptor* desc = i86_get_ir(intno);
	if(!desc)
		return 0;
	uint32_t addr = desc->baseLo | (desc->baseHi << 16 );

	I86_IRQ_HANDLER irq = (I86_IRQ_HANDLER)addr;
	
	return irq;
}

// Returns the cpu vender
const char* get_cpu_vendor(){

	return i86_get_cpu_vendor();

}

// Return the current tick count
int get_tick_count(){

	return i86_pit_get_tick_count();

}
