#include <string.h>
#include "vmm.h"
#include "pmm.h"

#define PTABLE_ADDR_SPACE_SIZE 0x400000		// Page Table = 4MB addr space
#define DTABLE_ADDR_SPACE_SIZE 0x100000000 	// Directory Table = 4GB addr space

#define PAGE_SIZE 4096	// Page Size = 4kb

pdirectory* _cur_directory=0;	// current directory table 
physical_addr _cur_pdbr=0;	// current page directory base register

inline pt_entry* vmm_ptable_lookup_entry(ptable* p,virtual_addr addr){
	if(p)
		return &p->m_entries[PAGE_TABLE_INDEX(addr)];
	return 0;
}

inline pd_entry* vmm_pdirectory_lookup_entry (pdirectory* p,virtual_addr addr){
	if(p)
		return &p->m_entries[PAGE_TABLE_INDEX(addr)];
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

void vmm_map_page(void* phys, void* virt){
	pdirectory* pageDirectory = vmm_get_directory();  // Get page dir
	pd_entry* e = &pageDirectory->m_entries[PAGE_DIRECTORY_INDEX((uint32_t)virt)]; // Get page tab
	if((*e & I86_PTE_PRESENT) != I86_PTE_PRESENT){ // if the page tab is not present, we need allocate it
		ptable* table = (ptable*) pmm_alloc_block();
		if(!table)
			return;
		memset(table,0,sizeof(ptable)); // Clear page tab
		pd_entry* entry = &pageDirectory->m_entries [PAGE_DIRECTORY_INDEX((uint32_t)virt)]; // Create a new entry
		// Map the page table and add it to the dir
		pd_entry_add_attrib(entry,I86_PDE_PRESENT);
		pd_entry_add_attrib(entry,I86_PDE_WRITABLE);
		pd_entry_set_frame(entry,(physical_addr)table);
	}
	ptable* table = (ptable*) PAGE_GET_PHYSICAL_ADDRESS(e); // get the table
	pt_entry* page = &table->m_entries[PAGE_TABLE_INDEX((uint32_t)virt)]; // get the page
	// Map the page to the table
	pt_entry_set_frame(page,(physical_addr)phys);
	pt_entry_add_attrib(page,I86_PTE_PRESENT);
}

void vmm_initialize(){
	ptable* table = (ptable*) pmm_alloc_block(); // default page table
	if(!table)
		return;

	ptable* table2 = (ptable*) pmm_alloc_block(); // 3GB page table
	if(!table2)
		return;
	
	memset(table,0,sizeof(ptable)); // clear the page table
	// We need to identity-map the 1st 4MB
	int i , frame,virt;
	for(i=0, frame=0x0,virt=0x00000000;i<1024;i++,frame+=4096,virt+=4096){
		// We need to create a new page and add it to the table2 page table		
		pt_entry page = 0;
		pt_entry_add_attrib(&page,I86_PTE_PRESENT);
		pt_entry_set_frame(&page,frame);
		table2->m_entries[PAGE_TABLE_INDEX(virt)] = page;
	}

	// Create the default directory table
	pdirectory* dir = (pdirectory*) pmm_alloc_blocks(3);
	if(!dir)
		return;

	memset(dir,0,sizeof(pdirectory)); // Clear it

	// set the first entry of directory table to point to our table
	pd_entry* entry = &dir->m_entries[PAGE_DIRECTORY_INDEX(0xc0000000)];
	pd_entry_add_attrib(entry,I86_PDE_PRESENT);
	pd_entry_add_attrib(entry,I86_PDE_WRITABLE);
	pd_entry_set_frame(entry,(physical_addr)table);
	// set the second entry of directory table to point to our table2
	pd_entry* entry2 = &dir->m_entries[PAGE_DIRECTORY_INDEX(0x00000000)];
	pd_entry_add_attrib(entry2,I86_PDE_PRESENT);
	pd_entry_add_attrib(entry2,I86_PDE_WRITABLE);
	pd_entry_set_frame(entry2,(physical_addr)table2);

	_cur_pdbr = (physical_addr) &dir->m_entries; // store current pdbr

	// switch to our page dir and enable paging
	vmm_switch_pdirectory(dir);
	pmm_paging_enable(1);
	
}
