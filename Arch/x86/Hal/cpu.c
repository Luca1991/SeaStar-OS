#include "cpu.h"
#include "gdt.h"
#include "idt.h"

int i86_cpu_initialize(){

	// Install GlobalDescriptorTable
	i86_gdt_initialize();
	// Install InterruptDescriptorTable
	i86_idt_initialize(0x8);


	return 0;
}


void i86_cpu_shutdown(){

	//nothing to do yet!

}

char* i86_get_cpu_vendor(){


        static char name[13] = {0};
	asm volatile("mov $0, %%eax \n"
		     "cpuid \n"
		     "mov %%ebx, %0 \n"
		     "mov %%edx, %1 \n"
		     "mov %%ecx, %2 \n"          
                    :"=m"(name), "=m"(*(name+4)),"=m"(*(name+8)));

        return name;
	
}


