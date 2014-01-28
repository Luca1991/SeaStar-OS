#include <Hal.h>
#include <KernelDisplay.h>

#define TOTAL_SYSCALLS 1


void* _syscalls[TOTAL_SYSCALLS] = {
	kernelPuts
};

void syscall_dispatcher(){
	int eax = 0;
	int ret = 0;
	asm ("movl %%eax,%0"
	     : "=r"(eax));   // Read eax register

	if(eax>(TOTAL_SYSCALLS-1))
		return;
	

	void* fnct = _syscalls[eax]; // Select the syscall
 	
	
	 asm volatile (" \
      			push %%edi; \
      			push %%esi; \
      			push %%edx; \
     			push %%ecx; \
      			push %%ebx; \
      			call *%0; \
      			pop %%ebx; \
      			pop %%ebx; \
      			pop %%ebx; \
     			pop %%ebx; \
     			pop %%ebx; \
    			" : "=a" (ret) :  "r" (fnct)); // Execute the syscall

	asm ("movl %0,%%eax"     
	    : : "r"(ret)); // Save the return value into eax


}

#define I86_IDT_DESC_RING3 0x60

void syscall_init(){
	setvect(0x80,syscall_dispatcher,I86_IDT_DESC_RING3);
}
