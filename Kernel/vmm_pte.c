#include "vmm_pte.h"



inline void pt_entry_add_attrib(pt_entry* e, uint32_t attrib){
	*e |= attrib;
} 

inline void pt_entry_del_attrib(pt_entry* e, uint32_t attrib){
	*e &= ~attrib;
} 

inline void pt_entry_set_frame (pt_entry* e, physical_addr addr){
	*e = (*e & ~I86_PTE_FRAME) | addr;
}

inline uint8_t pt_entry_is_present (pt_entry e){
	return e & I86_PTE_PRESENT;
}

inline uint8_t pt_entry_is_writable (pt_entry e){
	return e & I86_PTE_WRITABLE;
}

inline physical_addr pt_entry_pfn (pt_entry e){
	return e & I86_PTE_FRAME;
}
