#ifndef _GDT_H_INCLUDED
#define _GDT_H_INCLUDED


#include  <stdint.h>


#define MAX_DESCRIPTORS 3

// GDT descriptor acess bit flags!

#define I86_GDT_DESC_ACCESS		0x0001		// 00000001	Set Access Bit 

#define I86_GDT_DESC_READWRITE		0x0002		// 00000010	Descriptor is readable and writable

#define I86_GDT_DESC_EXPANSION		0x0004		// 00000100	Expansion direction bit

#define I86_GDT_DESC_EXEC_CODE		0x0008		// 00001000	Executable Code Segment

#define I86_GDT_DESC_CODEDATA 		0x0010 		// 00010000	Set Code or Data Descriptor

#define I86_GDT_DESC_DPL		0x0060		// 01100000	Set dpl bits

#define I86_GDT_DESC_MEMORY		0x0080		// 10000000	Set "in memory" bit

// GDT descriptor granularity bit flags!

#define I86_GDT_GRAND_LIMITHI_MASK      0x0f		// 00001111	Masks out LimitHi (High 4 bits of limit)

#define I86_GDT_GRAND_OS		0x10		// 00010000 	Set OS defined bit

#define I86_GDT_GRAND_32BIT		0x40		// 01000000	Set if 32 bit (Default is 16 bit)

#define I86_GDT_GRAND_4K		0x80		// 10000000	4KB Granularity



#define __packed
struct gdt_descriptor{
	
	uint16_t	limit;		// Bits 0-15 Segment Limit

	uint16_t	baseLo;		// Bits 0-23 of
	uint8_t		baseMid;	// Base Address
	
	uint8_t		flags;		// Descriptor access 
	uint8_t		grand;		//	 flags

	uint8_t		baseHi;		// Bits 24-32 of the base address
}__attribute((packed));
#undef __packed

typedef struct gdt_descriptor gdt_descriptor;


// Setup a descriptor in GDT
extern void gdt_set_descriptor(uint32_t i, uint64_t base,uint64_t limit, uint8_t access, uint8_t grand);

// Return a descriptor
extern gdt_descriptor* i86_gdt_get_descriptor(int i);

// Initialize the GDT
extern int i86_gdt_initialize();

#endif
