#ifndef _VMM_INCLUDE
#define _VMM_INCLUDE

#include <stdint.h>
#include "vmm_pte.h"
#include "vmm_pde.h"

typedef uint32_t virtual_addr;

// Defined by i86 Arch
#define PAGES_PER_TABLE 1024
#define PAGES_PER_DIR	1024

#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)

struct ptable{
	pt_entry m_entries[PAGES_PER_TABLE];
};

struct pdirectory{
	pd_entry m_entries[PAGES_PER_DIR];
};

typedef struct ptable ptable;
typedef struct pdirectory pdirectory;

extern void MmMapPage(void* phys,void* virt);

extern void vmm_initialize();

extern uint8_t vmm_alloc_page(pt_entry*);

extern void vmm_free_page(pt_entry* e);

extern uint8_t vmm_switch_pdirectory(pdirectory*);

extern pdirectory* vmm_get_directory();

extern void vmm_ptable_clear(ptable* p);

extern uint32_t vmm_ptable_virt_to_index (virtual_addr addr);

extern pt_entry* vmm_ptable_lookup_entry (ptable* p, virtual_addr addr);

extern uint32_t vmm_pdirectory_virt_to_index(virtual_addr addr);

extern void vmm_pdirecotry_clear(pdirectory* dir);

extern pd_entry* vmm_pdirectory_lookup_entry (pdirectory* p, virtual_addr addr);



#endif
