#ifndef _VMM_PDE
#define _VMM_PDE

#include <stdint.h>
#include "pmm.h"

// i86 Arch format
enum PAGE_PDE_FLAGS{
	I86_PDE_PRESENT = 1,
	I86_PDE_WRITABLE = 2,
	I86_PDE_USER = 4,
	I86_PDE_PWT = 8,
	I86_PDE_PCD = 0x10,
	I86_PDE_ACCESSED = 0x20,
	I86_PDE_DIRTY = 0x40,
	I86_PDE_4MB = 0x80,
	I86_PDE_CPU_GLOBAL = 0x100,
	I86_PDE_LV4_GLOBAL = 0x200,
	I86_PDE_FRAME = 0x7FFFF000

};

typedef uint32_t pd_entry;

extern void pd_entry_add_attrib(pd_entry* e, uint32_t attrib);

extern void pd_entry_del_attrib(pd_entry* e, uint32_t attrib);

extern void pd_entry_set_frame(pd_entry*, physical_addr);

extern uint8_t pd_entry_is_present(pd_entry e);

extern uint8_t pd_entry_is_user(pd_entry e);

extern uint8_t pd_entry_is_4mb(pd_entry e);

extern uint8_t pd_entry_is_writable(pd_entry e);

extern physical_addr pd_entry_pfn (pd_entry e);

extern void pd_entry_enable_global (pd_entry e);


#endif
