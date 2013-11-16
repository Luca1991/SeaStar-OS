#include <Hal.h>
#include "KernelDisplay.h"
#include "exception.h"



void kmain (void* MultibootStructure)
{


	hal_initialize ();


	setvect (0,divide_by_zero_fault);
	setvect (1,single_step_trap);
	setvect (2,nmi_trap);
	setvect (3,breakpoint_trap);
	setvect (4,overflow_trap);
	setvect (5,bounds_check_fault);
	setvect (6,invalid_opcode_fault);
	setvect (7,no_device_fault);
	setvect (8,double_fault_abort);
	setvect (10,invalid_tss_fault);
	setvect (11,no_segment_fault);
	setvect (12,stack_fault);
	setvect (13,general_protection_fault);
	setvect (14,page_fault);
	setvect (16,fpu_fault);
	setvect (17,alignment_check_fault);
	setvect (18,machine_check_abort);
	setvect (19,simd_fpu_fault);

	
	

	kernelClrScr (0x00); // b1 

	kernelGotoXY (0,0);
	kernelSetColor (0x1a);
	kernelPrintf (" SeaStar OS Preparing to boot...                      ");
	kernelGotoXY (0,1);
	kernelSetColor (0x0f); //b1
	kernelPrintf (" Benvenuto SeaStar OS - Programmato da Luca D'Amico\n");
	kernelGotoXY (0,2);
	kernelPrintf (" SeaStar OS e' un sistema operativo scritto da 0\n");
	kernelGotoXY (0,3);
	kernelPrintf (" Verra' presentato come tesi di laurea nel 2014\n");
	kernelGotoXY (0,4);
	kernelSetColor (0x2a);
	kernelPrintf (" SeaStar is Loading HAL...\n");
	kernelSetColor (0x1a);
	kernelGotoXY (0,24);
	kernelPrintf (" Initializing Hardware Abstraction Layer (HAL.o)...   ");
	
        kernelSetColor (0x0f);	
	
	kernelGotoXY (0,13);
	kernelPrintf (" SE RIESCI A LEGGERE QUESTO MESSAGGIO GLI \n INTERRUPT HARDWARE FUNZIONANO!!!");
	kernelGotoXY(0,16);		
	kernelPrintf(" CPU id: ");
        kernelPrintf(get_cpu_vendor());
	

	for (;;){
		
		kernelGotoXY (0,15);
		kernelPrintf (" Current tick count: %i", get_tick_count());

	}

}
