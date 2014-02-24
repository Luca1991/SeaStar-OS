#include <string.h>
#include "gdt.h"
#include "tss.h"
#include <KernelDisplay.h>

static tss_entry TSS;

void tss_set_stack(uint16_t kernelSS, uint16_t kernelESP){
	TSS.ss0 = kernelSS;
	TSS.esp0 = kernelESP;
}

// Install Task State Segmentent descriptor
void install_tss(uint32_t idx, uint16_t kernelSS, uint16_t kernelESP){
	uint32_t base = (uint32_t) &TSS;
	// Install descriptor in GDT
	gdt_set_descriptor(idx,base,base+sizeof(tss_entry),I86_GDT_DESC_ACCESS|I86_GDT_DESC_EXEC_CODE|
		I86_GDT_DESC_DPL|I86_GDT_DESC_MEMORY,0);

	// Init TSS
	memset((void*)&TSS, 0, sizeof(tss_entry));

	// Set stack and segment
	TSS.ss0 = kernelSS;
	TSS.esp0 = kernelESP;
	TSS.cs = 0x0b;
	TSS.ss = 0x13;
	TSS.es = 0x13;
	TSS.ds = 0x13;
	TSS.fs = 0x13;
	TSS.gs = 0x13;
	
	// Flush TSS (This function is written in ASM)
	flush_tss();


		
}
