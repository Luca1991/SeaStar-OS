#include "vmm.h"
#include <liballoc.h>
#include <size_t.h>

#define HEAP_START_ADDR 0xD0000000


uint32_t find_free_contiguous_pages(uint32_t startAddr,/*uint32_t length,*/ int num_pag){
	pdirectory* dir = vmm_get_directory ();
	pd_entry* pagedir = dir->m_entries;
	int cfreepage = 0;
	int cusedpage = 0;
	uint32_t firstFreePageAddr = startAddr;
	uint32_t x = 0, y = 0;
	for(x = startAddr; /*x < (startAddr+length)*/ ; x+=4096*1024){ // check 1 pagetable at a time
		if(pagedir[x>>22] != 0){   // check if pagetable exist
			ptable* table = (ptable*)(pagedir[x>>22] & 0xFFFFF000);
			for(y=x;;y+=4096){
				pt_entry* page = (pt_entry*)table->m_entries [ PAGE_TABLE_INDEX (y) ]; 
		//		kernelPrintf("current page present value %d\n",(uint32_t)page & 0x1);
				if(((uint32_t)(page) & 0x1) != 0){ // check if a page is present 
					cusedpage++;
			//		kernelPrintf("Found used page number: %d\n",PAGE_TABLE_INDEX (y));
					cfreepage = 0;   // a present page was found, so reset the contiguous pages counter
					firstFreePageAddr = y+4096; // set firstFreePageAddr to the next page (if not free, it will be corrected next iteration)
				}
				else{
					cfreepage++;
			//		kernelPrintf("Found free page number: %d\n",PAGE_TABLE_INDEX (y));
					if(cfreepage>=num_pag)
						return firstFreePageAddr;
				}
				if(PAGE_TABLE_INDEX (y)==1023) break;
			}
		}
		else{ // if a pagetable doesn't exist add 1024 free pages to the counter
		//	kernelPrintf("found free pagetable! (1024 free pages) at %x\n",x);
			cfreepage+=1024;
			if(cfreepage>=num_pag)
				return firstFreePageAddr;
			
		}
	}
//	kernelPrintf("Used Pages Found: %d\n",cusedpage);	
//	kernelPrintf("Free Pages Found: %d\n",cfreepage);	
	return -1;
}
#ifndef NULL
#define NULL 0
#endif
void* liballoc_alloc(size_t num_pag){
	int i = 0;	
	uint32_t virtAddr = find_free_contiguous_pages(HEAP_START_ADDR,num_pag); // Find num_pag contiguous free pages
	
	if(virtAddr==-1)
		return NULL; // Unable to find num_pag contiguos free pages

	for(i=0;i<num_pag;i++){
		void* p = pmm_alloc_block(); // allock a block of physical memory
		if(!p)
			goto fail; 
		vmm_mapPhysicalAddr(vmm_get_directory(), virtAddr+i*0x1000,(uint32_t)p,I86_PTE_PRESENT|I86_PTE_WRITABLE);
	}
	
	return (void*)virtAddr; // Ok, just return the starting address of the mapped pages

	fail:
		return NULL;

}

int liballoc_free(void* virtAddr,size_t num_pag){
	int i = 0;
	for(i=0;i<num_pag;i++){
		vmm_unmapPhysicalAddr(vmm_get_directory(),(uint32_t)virtAddr+i*0x1000);
	}
	return 0;
}



int liballoc_lock(){
	asm("cli");
	return 0;
}

int liballoc_unlock(){
	asm("sti");
	return 0;
}
