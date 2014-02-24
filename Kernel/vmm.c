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


int vmm_initialize(){
	ptable* table = (ptable*) pmm_alloc_block(); // default page table
	if(!table)
		return 0;
	
	ptable* table2 = (ptable*) pmm_alloc_block(); // 3gb page table
	if(!table2)
		return 0;

	vmm_ptable_clear(table); // clear the default page table
	vmm_ptable_clear(table2);// clear the 3gb page table

	// We need to identity-map the 1st 4MB
	int i , frame,virtual;
	for(i=0, frame=0,virtual=0x00000000;i<1024;i++,frame+=4096,virtual+=4096){ 
		// We need to create a new page and add it to the table2 page table		
		pt_entry page = 0;
		pt_entry_add_attrib(&page,I86_PTE_PRESENT);
		pt_entry_set_frame(&page,frame);
		table2->m_entries[PAGE_TABLE_INDEX(virtual)] = page;
	}

	// Map 0MB to 3GB
	for(i=0, frame=0x00000,virtual=0xc0000000;i<1024;i++,frame+=4096,virtual+=4096){
		// We need to create a new page and add it to the table page table		
		pt_entry page = 0;
		pt_entry_add_attrib(&page,I86_PTE_PRESENT);
		pt_entry_set_frame(&page,frame);
		table->m_entries[PAGE_TABLE_INDEX(virtual)] = page;
	}

	// Create the default directory table..
	pdirectory* dir = (pdirectory*) pmm_alloc_blocks(3);
	if(!dir)
		return 0;

	vmm_pdirectory_clear(dir); // .. and clear it 

	// set the first entry of directory table to point to our table
	pd_entry* entry = &dir->m_entries [PAGE_DIRECTORY_INDEX(0xc0000000)];
	pd_entry_add_attrib(entry,I86_PDE_PRESENT);
	pd_entry_add_attrib(entry,I86_PDE_WRITABLE);
	pd_entry_set_frame(entry,(physical_addr)table);

	// set the second entry of directory table to point to table2
	pd_entry* entry2 = &dir->m_entries [PAGE_DIRECTORY_INDEX(0x00000000)];
	pd_entry_add_attrib(entry2,I86_PDE_PRESENT);
	pd_entry_add_attrib(entry2,I86_PDE_WRITABLE);
	pd_entry_set_frame(entry2,(physical_addr)table2);


	_cur_pdbr = (physical_addr) &dir->m_entries; // store current pdbr

	// switch to our page dir and enable paging
	if(vmm_switch_pdirectory(dir)==0)
		return 0;
	pmm_paging_enable(1);
	return 1;
	
}

// Create Page Table
int vmm_createPageTable(pdirectory* dir, uint32_t virtual,uint32_t flags){
	pd_entry* pagedir = dir->m_entries;
	if(pagedir[virtual>>22]==0){ // if the pagetable doesn't exist, create it!
		void* block = pmm_alloc_block();
		if(!block)
			return 0;
		pagedir[virtual>>22] = ((uint32_t)block)|flags;
		memset((uint32_t*)pagedir[virtual>>22],0,4096); // clear it
		vmm_mapPhysicalAddr(dir,(uint32_t)block,(uint32_t)block,flags); // map pagetable into dir
	}
	return 1;
}

// Map physical address to virtual address
void vmm_mapPhysicalAddr(pdirectory* dir, uint32_t virtual,uint32_t physical,uint32_t flags){
	pd_entry* pagedir = dir->m_entries;
	if(pagedir[virtual>>22]==0)
		vmm_createPageTable(dir,virtual,flags);
	((uint32_t*)(pagedir[virtual>>22] & ~0xfff))[virtual<<10>>10>>12] = physical|flags; 
}

// Unmap page table
void vmm_unmapPageTable(pdirectory* dir,uint32_t virtual){
	pd_entry* pagedir = dir->m_entries;
	if(pagedir[virtual>>22] != 0){ // Check if pagetable exist
		void* frame = (void*)(pagedir[virtual>>22] & 0x7FFFF000); // Get mapped frame
		pmm_free_block(frame);
		pagedir[virtual>>22]=0; // Unmap frame
	}
}

// Unmap Physical address
void vmm_unmapPhysicalAddr(pdirectory* dir,uint32_t virtual){
	pd_entry* pagedir = dir->m_entries;
	if(pagedir[virtual>>22] != 0){
		vmm_unmapPageTable(dir,virtual);
	} 
}

// Create Address Space
pdirectory* vmm_createAdressSpace(){
	pdirectory* dir = 0;
	dir = (pdirectory*) pmm_alloc_block(); // Allocate page directory..
	if(!dir)
		return 0;
	vmm_pdirectory_clear(dir); // ..and clean it
	return dir;
}

// Get Physical Address
void* vmm_getPhysicalAddr(pdirectory* dir, uint32_t virtual){
	pd_entry* pagedir = dir->m_entries;
	if(pagedir[virtual>>22]==0)
		return 0;

	return (void*)((uint32_t*)(pagedir[virtual>>22] & ~0xfff))[virtual<<10>>10>>12]; 
}

