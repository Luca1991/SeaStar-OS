#include <string.h>
#include <fs/vfs.h>
#include "vmm.h"
#include "task.h"
#include <KernelDisplay.h>
#include <elf32.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

#define PROC_INVALID_ID -1

static process _proc = {
	PROC_INVALID_ID,0,0,0,0
};


process* getCurrentProcess(){
	return &_proc; // Return Current Process
}

// Map kernel space into address space
void mapKernelSpace(pdirectory* addressSpace){
	uint32_t virtualAddr;
	uint32_t physicalAddr;

	int flags = I86_PTE_PRESENT|I86_PTE_WRITABLE; // Default flags (prevent user mode access)

	vmm_mapPhysicalAddr(addressSpace,0x8000,0x8000,flags); // Map kernel stack space at 0x8000
	vmm_mapPhysicalAddr(addressSpace,0x9000,0x9000,flags); // Map kernel stack space at 0x9000

	virtualAddr = 0xc0000000;
	physicalAddr = 0x100000;
	uint32_t i;
	for(i=0;i<10;i++) // Map kernel Image (7 pages at physical 1MB virtual 3GB)
		vmm_mapPhysicalAddr(addressSpace, virtualAddr+(i*PAGE_SIZE), physicalAddr+(i*PAGE_SIZE), flags);
	
	virtualAddr = 0xa0000;
	physicalAddr = 0xa0000;
	for(i=0;i<31;i++) // Map display memory for debug minidriver
		vmm_mapPhysicalAddr(addressSpace,virtualAddr+(i*PAGE_SIZE),physicalAddr+(i*PAGE_SIZE),flags);

	// Map page directory into address space
	vmm_mapPhysicalAddr(addressSpace, (uint32_t)addressSpace, (uint32_t)addressSpace, I86_PTE_PRESENT|I86_PTE_WRITABLE);
}


int createProcess(char* imagePath){

	kernelPrintf("\nDEBUG: imagePath=%s\n",imagePath); // Questa è la path (tipo A:\mioelf.elf)
	FILE file;
	pdirectory* addressSpace = 0;
	process* proc = 0;
	thread* mainThread = 0;
	unsigned char* memory = 0;
	unsigned char buf[512];
	uint32_t i = 0;
	Elf32_Ehdr *hdr;
	Elf32_Phdr *p_entry;

	file=volOpenFile(imagePath,imagePath[0]);
	if(file.flags == FS_INVALID)
		return 0;
	if((file.flags & FS_DIR) == FS_DIR)
		return 0;

	volReadFile(&file,buf,512);
	
	int is_elf = elf_check_support((char*)buf);
	if(is_elf!=1)
		return 0;

	hdr = (Elf32_Ehdr *) buf;
	p_entry = (Elf32_Phdr *) (buf + hdr->e_phoff);

	addressSpace = vmm_get_directory();
	if(!addressSpace){
		volCloseFile(&file);
		return 0;
	}
	
	proc = getCurrentProcess();
	proc->id = 1;
	proc->pageDirectory = addressSpace;
	proc->priority = 1;
	proc->state = PROCESS_STATE_ACTIVE;
	proc->threadCount = 1;

	mainThread = &proc->threads[0];
	mainThread->kernelStack = 0;
	mainThread->parent = proc;
	mainThread->priority = 1;
	mainThread->state = PROCESS_STATE_ACTIVE;
	mainThread->initialStack = 0;
	mainThread->stackLimit = (void*)((uint32_t) mainThread->initialStack+4096);
	mainThread->imageBase = p_entry->p_vaddr;
	mainThread->imageSize =  p_entry->p_vaddr + p_entry->p_memsz;
	memset(&mainThread->frame,0,sizeof(trapFrame));
	mainThread->frame.eip = hdr->e_entry + p_entry->p_vaddr;
	mainThread->frame.flags = 0x200; 


	memory = (unsigned char*)pmm_alloc_block();
	memset(memory,0,4096);
	memcpy(memory,buf,512);

	for(i=1;i<=mainThread->imageSize/512;i++){
		if(file.eof == 1)
			break;
		volReadFile(&file,memory+512*i,512);
	}

	vmm_mapPhysicalAddr(proc->pageDirectory,p_entry->p_vaddr,(uint32_t)memory,I86_PTE_PRESENT|I86_PTE_WRITABLE|I86_PTE_USER);

	i = 1;
	
	while(file.eof != 1){
		unsigned char* cur = (unsigned char*)pmm_alloc_block();
		int curBlock = 0;
		for(curBlock=0; curBlock<8;curBlock++){
			if(file.eof == 1)
				break;
			volReadFile(&file,cur+512*curBlock,512);
		}

		vmm_mapPhysicalAddr(proc->pageDirectory,p_entry->p_vaddr+i*4096,(uint32_t) cur,I86_PTE_PRESENT|I86_PTE_WRITABLE|I86_PTE_USER);
	
		i++;
	}

	void* stack = (void*)(p_entry->p_vaddr+(p_entry->p_vaddr + p_entry->p_memsz)+PAGE_SIZE);
	void* stackPhys = (void*)pmm_alloc_block();

	vmm_mapPhysicalAddr(addressSpace,(uint32_t)stack,(uint32_t)stackPhys,I86_PTE_PRESENT|I86_PTE_WRITABLE|I86_PTE_USER);	

	mainThread->initialStack = stack;
	mainThread->frame.esp = (uint32_t)mainThread->initialStack;
	mainThread->frame.ebp = mainThread->frame.esp;

	volCloseFile(&file);

	return proc->id;	
}

void executeProcess(){ // FIXME TODO: this function is broken, and I don't know why yet. Need more testing.
	process* proc = 0;
	int entryPoint = 0;
	unsigned int procStack = 0;
	
	proc = getCurrentProcess();
	if(proc->id==PROC_INVALID_ID)
		return;
	if(!proc->pageDirectory)
		return;

	entryPoint = proc->threads[0].frame.eip;
	procStack = proc->threads[0].frame.esp;
	
	asm("cli");
	pmm_load_PDBR((physical_addr)proc->pageDirectory);


	asm volatile("  \
		      cli; \
		      mov $0x23, %%ax; \
		      mov %%ax, %%ds; \
		      mov %%ax, %%es; \
		      mov %%ax, %%fs; \
		      mov %%ax, %%gs; \
      		      push $0x23; \
     	 	      push $0x0; \
		      push $0x200; \
		      pushl $0x1B; \
		      push %0; \
		      iret; \
		      ": : "b"(entryPoint));


}


