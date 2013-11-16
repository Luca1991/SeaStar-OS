#ifndef _IDT_H_INCLUDED
#define _IDT_H_INCLUDED

#include <stdint.h>

#define I86_MAX_INTERRUPTS	256 //x86 have 256 possible interrupt handler (from 0 to 255)

// Format 0D110 where D = descriptor type 
#define I86_IDT_DESC_BIT16	0x06	// 00000110 - 16Bit Mode
#define I86_IDT_DESC_BIT32	0x0E	// 00001110 - 32Bit Mode
#define I86_IDT_DESC_RING1	0x40	// 01000000 - 01 = RING1
#define I86_IDT_DESC_RING2	0x20	// 00100000 - 10 = RING2	
#define I86_IDT_DESC_RING3	0x60	// 01100000 - 11 = RING3
#define I86_IDT_DESC_PRESENT	0x80	// 10000000 - 1 = Present (0 = Not Present)


// ATTENTION: Interrupt handlers are called by the CPU. The stack setup may change, so we leave it up to the interrupts' implementation to handle it and properly return!!
typedef void (*I86_IRQ_HANDLER)(void);


#define __packed
struct idt_descriptor{

	uint16_t baseLo; 	// Bits 0-16 of IR address (IR = Interrupt Routine)
	uint16_t sel;    	// Code Selector in GDT
	uint8_t  reserved;	// This is reserved (should be 0)
	uint8_t  flags;	 	// Bit Flags
	uint16_t baseHi;	// Bits 16-32 of IR address

}__attribute((packed));
#undef __packed

extern struct idt_descriptor* i86_get_ir(uint32_t i); // Returns Interrupt Descriptor

extern int i86_install_ir(uint32_t i, uint16_t flags, uint16_t sel, I86_IRQ_HANDLER); // Install Interrupt handler (this will be our callback function for INT )

extern int i86_idt_initialize(uint16_t codeSel); // Initialize IDT

#endif
