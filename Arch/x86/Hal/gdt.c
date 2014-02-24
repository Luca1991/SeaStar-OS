#include "gdt.h"
#include <string.h>

#define __packed
struct gdtr{
	uint16_t m_limit;	//Size of the GDT
	uint32_t m_base;	//Base of the GDT (address)	
}__attribute((packed));
#undef __packed
 


static struct gdt_descriptor _gdt[MAX_DESCRIPTORS]; //GDT in an array of descriptors
static struct gdtr _gdtr;			    // GDTR Data	


static void gdt_install();

extern void gdt_fix();

static void gdt_install(){
	asm ("lgdt (%0)": :"m"(_gdtr));
	gdt_fix();

}


// Setup a descriptor in GDT
void gdt_set_descriptor(uint32_t i, uint64_t base,uint64_t limit, uint8_t access, uint8_t granularity){
	if(i>MAX_DESCRIPTORS)
		return;

	memset((void*)&_gdt[i],0,sizeof(gdt_descriptor)); // Clean the descriptor
	// Set limit and base addr
	_gdt[i].baseLo = (uint16_t)(base & 0xffff);
	_gdt[i].baseMid = (uint8_t)((base >> 16) & 0xff);
	_gdt[i].baseHi = (uint8_t) ((base >> 24) & 0xff);
	_gdt[i].limit = (uint16_t)(limit & 0xffff);
	// Set flags and granularity bytes
	_gdt[i].flags = access;
	_gdt[i].gran = (uint8_t)((limit >> 16) & 0x0f);
	_gdt[i].gran |= granularity & 0xf0;

}

// Return a descriptor in GDT
gdt_descriptor* i86_gdt_get_descriptor(int i){
	if(i>MAX_DESCRIPTORS)
		return 0;
	return &_gdt[i];

}

// Initialize GDT
int i86_gdt_initialize(){
	// Setup our gdtr
	_gdtr.m_limit = (sizeof(struct gdt_descriptor) * MAX_DESCRIPTORS)-1;
	_gdtr.m_base = (uint32_t) &_gdt[0];

	// Set null descriptor
	gdt_set_descriptor(0,0,0,0,0);
	// Set default code descriptor
	gdt_set_descriptor (1,0,0xffffffff,I86_GDT_DESC_READWRITE|I86_GDT_DESC_EXEC_CODE|I86_GDT_DESC_CODEDATA|I86_GDT_DESC_MEMORY,I86_GDT_GRAN_4K | 			I86_GDT_GRAN_32BIT | I86_GDT_GRAN_LIMITHI_MASK);
	// Set default data descriptor
	gdt_set_descriptor (2,0,0xffffffff,
		I86_GDT_DESC_READWRITE|I86_GDT_DESC_CODEDATA|I86_GDT_DESC_MEMORY,
		I86_GDT_GRAN_4K | I86_GDT_GRAN_32BIT | I86_GDT_GRAN_LIMITHI_MASK);
	// Set default usermode code descriptor
	gdt_set_descriptor(3,0,0xffffffff,
		I86_GDT_DESC_READWRITE|I86_GDT_DESC_EXEC_CODE|I86_GDT_DESC_CODEDATA|
		I86_GDT_DESC_MEMORY|I86_GDT_DESC_DPL, I86_GDT_GRAN_4K| I86_GDT_GRAN_32BIT|
		I86_GDT_GRAN_LIMITHI_MASK);
	// Set default usermode data descriptor
	gdt_set_descriptor(4,0,0xffffffff,
		I86_GDT_DESC_READWRITE|I86_GDT_DESC_CODEDATA|I86_GDT_DESC_MEMORY|
		I86_GDT_DESC_DPL, I86_GDT_GRAN_4K|I86_GDT_GRAN_32BIT|I86_GDT_GRAN_LIMITHI_MASK);


	// install gdtr
	gdt_install();

	return 0;	
	
}	


