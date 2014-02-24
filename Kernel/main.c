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
#include <fs/ramdisk.h>
#include <shell/seashell.h>
#include "syscall.h"
#include "liballoc.h"

char *module_start;
unsigned int module_end;


void kmain (multiboot_info_t* MultibootStructure)
{


	extern  uint32_t end; // Trick from bootstrap / linking script to get the kernel end addr

	
	
	uint32_t kernelSize=(int)&end;
	kernelSize -= 0x100000; // Real Kernel Size for low half
	

	

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
	setvect (20,virtualization_exception,0);
	setvect (30,security_exception,0);

	
	
	kernelClrScr (0x00);
	
	kernelGotoXY (0,0);
	kernelSetColor (0x1a);
	kernelPrintf (" SeaStar OS Preparing to boot...                      ");
        

	


	kernelSetColor (0x0f);

	//! get memory size in KB
	uint32_t memSize = MultibootStructure->mem_lower + MultibootStructure->mem_upper; 


	// Init Physical Memory Manager
	pmm_init (memSize, 0x100000+kernelSize); 

;

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
	pmm_deinit_region (0x100000, kernelSize); 

	// deinit the region of memory for kernel stack (or for floppy DMA)
        pmm_deinit_region (0x0, 0x10000); 


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
			fsysFatInit('a'); // Mound Fat12 FS on device "A:\" 
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

		 
	install_tss(5,0x10,0x9000);	//FIXME
	kernelPrintf("TSS Installed.... \n");


	// LIBALLOC EXPERIMENTS (need more testing)

	/*uint32_t* cc = kcalloc(1,sizeof(int));
	kernelPrintf("DEBUG: cc %x \n",cc);
	
	uint32_t* dd = kcalloc(1,sizeof(int));
	kernelPrintf("DEBUG: dd %x \n",dd);

	kfree(dd);

	uint32_t* ee = kcalloc(1,sizeof(int));
	kernelPrintf("DEBUG: ee %x \n",ee);
	
	uint32_t* ff = kcalloc(1,sizeof(int));
	kernelPrintf("DEBUG: ff %x \n",ff);

	uint32_t* gg = kcalloc(1,sizeof(int));
	kernelPrintf("DEBUG: gg %x \n",gg);
	
	uint32_t* hh = kcalloc(1,sizeof(int));
	kernelPrintf("DEBUG: hh %x \n",hh);

	uint32_t* ii = kcalloc(1,sizeof(int));
	kernelPrintf("DEBUG: ii %x \n",ii);*/

	/*uint32_t* cc = kmalloc(10*sizeof(int));
	kernelPrintf("DEBUG: cc %x \n",cc);
	
	uint32_t* dd = kmalloc(10*sizeof(int));
	kernelPrintf("DEBUG: dd %x \n",dd);

	kfree(dd);

	uint32_t* ee = kmalloc(10*sizeof(int));
	kernelPrintf("DEBUG: ee %x \n",ee);
	
	uint32_t* ff = kmalloc(10*sizeof(int));
	kernelPrintf("DEBUG: ff %x \n",ff);

	uint32_t* gg = kmalloc(10*sizeof(int));
	kernelPrintf("DEBUG: gg %x \n",gg);

	uint32_t* hh = kmalloc(10*sizeof(int));
	kernelPrintf("DEBUG: hh %x \n",hh);*/

	module_start = (char*) *((unsigned int*)MultibootStructure->mods_addr);
	module_end = *((unsigned int*)(MultibootStructure->mods_addr+4));
	fsysRdInit(module_start,'x'); // Mound RamDisk FS on device "X:\" 

	kernelPrintf("RamDisk Filesystem mounted.. \n");
	
	kernelPrintf("Init Done.. Running SeaShell.... \n");
	SeaShell();

	while(1);


}

