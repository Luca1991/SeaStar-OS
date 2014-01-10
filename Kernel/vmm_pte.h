#ifndef _VMM_PTE
#define _VMM_PTE

#include <stdint.h>
#include "pmm.h"



// i86 Arch format
enum PAGE_PTE_FLAGS{
	I86_PTE_PRESENT = 1,
	I86_PTE_WRITABLE = 2,
	I86_PTE_USER = 4,
	I86_PTE_WRITETHOUGH = 8,
	I86_PTE_NOT_CACHEABLE = 0x10,
	I86_PTE_ACCESSED = 0x20,
	I86_PTE_DIRTY = 0x40,
	I86_PTE_PAT = 0x80,
	I86_PTE_CPU_GLOBAL = 0x100,
	I86_PTE_LV4_GLOBAL = 0x200,
	I86_PTE_FRAME = 0x7FFFF000

};

typedef uint32_t pt_entry;

extern void pt_entry_add_attrib(pt_entry* e, uint32_t attrib);

extern void pt_entry_del_attrib(pt_entry* e, uint32_t attrib);

extern void pt_entry_set_frame (pt_entry*, physical_addr);

extern uint8_t pt_entry_is_present (pt_entry e);

extern uint8_t pt_entry_is_writable (pt_entry e);

extern physical_addr pt_entry_pfn (pt_entry e);

#endif
