#include <string.h>
#include "vmm.h"
#include "pmm.h"

#define PTABLE_ADDR_SPACE_SIZE 0x400000		// Page Table = 4MB addr space
#define DTABLE_ADDR_SPACE_SIZE 0x100000000 	// Directory Table = 4GB addr space

#define PAGE_SIZE 4096	// Page Size = 4kb

pdirectory* _cur_directory=0;	// current directory table 
physical_addr _cur_pdbr=0;	// current page directory base register


inline uint32_t vmm_ptable_virt_to_index(virtual_addr addr){
	// Return index only if it doesn't exceed 4mb ptable addr space size
	return (addr >= PTABLE_ADDR_SPACE_SIZE) ? 0 : addr/PAGE_SIZE;
}

inline uint32_t vmm_pdirectory_virt_to_index(virtual_addr addr){
	// Return index only if it doesn't exceed 4gb pdirectory addr space size
	return (addr >= DTABLE_ADDR_SPACE_SIZE) ? 0 : addr/PAGE_SIZE;
}

inline void vmm_ptable_clear(ptable* p){
	if(p)
		memset(p,0,sizeof(ptable));
}

inline void vmm_pdirectory_clear(pdirectory* dir){
	if(dir)
		memset(dir,0,sizeof(pdirectory));
}

inline pt_entry* vmm_ptable_lookup_entry(ptable* p,virtual_addr addr){
	if(p)
		return &p->m_entries[vmm_ptable_virt_to_index(addr)];
	return 0;
}

inline pd_entry* vmm_pdirectory_lookup_entry (pdirectory* p,virtual_addr addr){
	if(p)
		return &p->m_entries[vmm_ptable_virt_to_index(addr)];
	return 0;
}

inline uint8_t vmm_switch_pdirectory(pdirectory* dir){
	if(!dir)
		return 0;
	_cur_directory = dir;
	pmm_load_PDBR (_cur_pdbr);
	return 1;

}

pdirectory* vmm_get_directory(){
	return _cur_directory;
}

uint8_t vmm_alloc_page (pt_entry* e){
	void* p = pmm_alloc_block();
	if(!p)
		return 0; 
	// Map the physical frame to the page
	pt_entry_set_frame (e,(physical_addr)p);
	pt_entry_add_attrib(e,I86_PTE_PRESENT);
	return 1;
}

void vmm_free_page(pt_entry* e){
	void* p = (void*) pt_entry_pfn (*e);
	if(p)
		pmm_free_block(p);
	pt_entry_del_attrib(e,I86_PTE_PRESENT);
}


void vmm_initialize(){
	ptable* table = (ptable*) pmm_alloc_block(); // default page table
	if(!table)
		return;

	vmm_ptable_clear(table); // clear the page table
	

	// We need to identity-map the 1st 4MB
	int i , frame;
	for(i=0, frame=0;i<1024;i++,frame+=4096){
		// We need to create a new page and add it to the table2 page table		
		pt_entry page = 0;
		pt_entry_add_attrib(&page,I86_PTE_PRESENT);
		pt_entry_add_attrib(&page,I86_PTE_USER);
		pt_entry_set_frame(&page,frame);
		table->m_entries[vmm_ptable_virt_to_index(frame)] = page;
	}

	// Create the default directory table..
	pdirectory* dir = (pdirectory*) pmm_alloc_blocks(3);
	if(!dir)
		return;

	vmm_pdirectory_clear(dir); // .. and clear it 

	// set the first entry of directory table to point to our table
	pd_entry* entry = vmm_pdirectory_lookup_entry(dir,0);
	pd_entry_add_attrib(entry,I86_PDE_PRESENT);
	pd_entry_add_attrib(entry,I86_PDE_WRITABLE);
	pd_entry_add_attrib(entry,I86_PDE_USER);
	pd_entry_set_frame(entry,(physical_addr)table);


	_cur_pdbr = (physical_addr) &dir->m_entries; // store current pdbr

	// switch to our page dir and enable paging
	vmm_switch_pdirectory(dir);
	pmm_paging_enable(1);
	
}
