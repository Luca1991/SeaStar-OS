#include <bootinfo.h>
#include <Hal.h>
#include <string.h>
#include <stdio.h>
#include <KernelDisplay.h>
#include "exception.h"
#include "pmm.h"
#include "vmm.h"
#include <drivers/keyboard_driver.h>
#include <drivers/floppy_driver.h>
#include <fs/fat12.h>
#include <shell/seashell.h>
#include "syscall.h"



void kmain (multiboot_info_t* MultibootStructure)
{

	extern  uint32_t end; // Trick from bootstrap / linking script to get the kernel end addr

	
	
	uint32_t kernelSize=(int)&end;
	kernelSize -= 0x100000; // Real Kernel Size
	



	hal_initialize ();


	setvect (0,divide_by_zero_fault,0);
	setvect (1,single_step_trap,0);
	setvect (2,nmi_trap,0);
	setvect (3,breakpoint_trap,0);
	setvect (4,overflow_trap,0);
	setvect (5,bounds_check_fault,0);
	setvect (6,invalid_opcode_fault,0);
	setvect (7,no_device_fault,0);
	setvect (8,double_fault_abort,0);
	setvect (10,invalid_tss_fault,0);
	setvect (11,no_segment_fault,0);
	setvect (12,stack_fault,0);
	setvect (13,general_protection_fault,0);
	setvect (14,page_fault,0);
	setvect (16,fpu_fault,0);
	setvect (17,alignment_check_fault,0);
	setvect (18,machine_check_abort,0);
	setvect (19,simd_fpu_fault,0);

	
	

	kernelClrScr (0x00);

	kernelGotoXY (0,0);
	kernelSetColor (0x1a);
	kernelPrintf (" SeaStar OS Preparing to boot...                      ");
        
	 /*
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

	}*/


	kernelSetColor (0x0f);

	//! get memory size in KB
	uint32_t memSize = MultibootStructure->mem_lower + MultibootStructure->mem_upper; 

	// Init Physical Memory Manager
	pmm_init (memSize, 0x100000+kernelSize*512);

	
	kernelPrintf("\nPhysical Memory Manager initialized\nwith %i KB physical memory; memLo: %i memHi: %i\n",
		memSize,MultibootStructure->mem_lower ,MultibootStructure->mem_upper);



//	kernelPrintf ("Physical Memory Manager is running:\n");
	memory_map_t*	region = (memory_map_t*)  MultibootStructure->mmap_addr;
	int i;

	for (i=0; i<10; ++i) {

		// if region type > 4 it is reserved (sanity check)
		if (region[i].type>4)
			break;


		// if start address is 0 there is no more entries
		if (i>0 && region[i].base_addr_low==0)
			break;

		
/*		kernelPrintf ("Memory Region %i: Initial addr: 0x%x%x Size in bytes: 0x%x%x type: %i \n", i, 
			region[i].base_addr_high, region[i].base_addr_low,
			region[i].length_high,region[i].length_low,
			region[i].type);*/

		// if region[i] is type 1, then it is available memory.. just initialize the region for use
		if (region[i].type==1)
			pmm_init_region (region[i].base_addr_low, region[i].length_low);
	}


	// deinit the region of memory that the kernel is using
	pmm_deinit_region (0x100000, kernelSize*512);

	// deinit the region of memory used for floppy DMA
//	pmm_deinit_region (0x0, 0x8000); // FIXME - DMA Fix... still need some test !



	kernelPrintf ("\npmm regions initialized: %i allocation blocks;\nused or reserved blocks: %i\nfree blocks: %i\n",
	pmm_get_block_count (),  pmm_get_used_block_count (), pmm_get_free_block_count () );

	kernelPrintf("Kernel Size %d \n",kernelSize);


	
	vmm_initialize ();

	int ok = pmm_is_paging();
	if (ok==0)
		kernelPrintf("Paging ENABLED!!!!!! \n");
	
	
	kkeyboard_install(33);
	kernelPrintf("Keyboard Driver Installed..  \n");
	int check_fdd;
	check_fdd = is_fdd_present();
	
	
	if(check_fdd){ 
		int check_timeout = 0 ;
		floppydisk_set_working_drive (0);
		check_timeout = floppydisk_install(38);
		if(check_timeout){
			floppydisk_set_dma(0x8000);
			kernelPrintf("Floppy Driver Installed.. \n");
			fsysFatInit();
			kernelPrintf("Virtual Filesystem Installed.. \n");
			kernelPrintf("FAT12 Filesystem Initialized.. \n");
		}
		else{   // else, floppy drive irq timeout!!
			kernelSetColor (0x04); 
			kernelPrintf("Floppy Disk Drive IRQ Timeout.. \n");
			kernelPrintf("Unable to Install VFS due to lack of physical medium.. \n");
			kernelSetColor (0x0f);			
		}
	}
	else if(!check_fdd){
		kernelSetColor (0x04); 
		kernelPrintf("Floppy Disk Drive not found.. \n");
		kernelPrintf("Unable to Install VFS due to lack of physical medium.. \n");
		kernelSetColor (0x0f);
	}
	
	
	

	syscall_init();
	kernelPrintf("SysCalls Initliazed.... \n");
	install_tss(5,0x10,0);	
	kernelPrintf("TSS Installed.... \n");
	kernelPrintf("Running SeaShell.... \n");
	SeaShell();

	while(1);


}

