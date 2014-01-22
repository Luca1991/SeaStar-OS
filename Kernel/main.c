#include <bootinfo.h>
#include <Hal.h>
#include <string.h>
#include <stdio.h>
#include "KernelDisplay.h"
#include "exception.h"
#include "pmm.h"
#include "vmm.h"
#include <drivers/keyboard_driver.h>
#include <drivers/floppy_driver.h>
#include <fs/fat12.h>



void sleep (int ms){
	int ticks = ms + get_tick_count();
	while(ticks > get_tick_count());
}

KEYCODE getch(){
	KEYCODE key = KEY_UNKNOWN;

	while(key==KEY_UNKNOWN)
		key = kkeyboard_get_last_key();

	kkeyboard_discard_last_key();
	return key;
}

void cmd(){
	kernelSetColor (0x1a);
	kernelPrintf("\nRoot@SeaStar#-Command>");
	kernelSetColor (0x0f);
	kernelPrintf(" ");
}

void get_cmd(char* buf,int n){
	
	KEYCODE key = KEY_UNKNOWN;
	int BufChar;

	int i = 0;
	while(i<n){
		BufChar = 1;
		key = getch();

		if(key == KEY_RETURN)
			break;
	
		if(key == KEY_BACKSPACE){
			BufChar = 0;
			if(i>0){
				unsigned x,y;
				kernelGetXY(&x,&y);
				if(x>0)
					kernelGotoXY (--x,y);
				else{
					y--;
					x = 80;
				}
	
				kernelPutc(' ');
				kernelGotoXY(x,y);
				i--;
			}

		}	
		if(BufChar==1){
			char c = kkeyboard_key_to_ascii(key);
			if ((c != 0) && (key != 0x1001)){
				kernelPutc(c);
				buf[i++] = c;
			}	
		}
		sleep(10);
	}
	buf[i] = '\0';
}

void cmd_read_sect(){
	uint32_t sectornum = 0;
	char sectornumbuf[4];
	uint8_t* sector = 0;
	kernelPuts("\n\rPlease select the sector to dump > ");
	get_cmd(sectornumbuf,3);
	sectornum = atoi(sectornumbuf);

	kernelPrintf("\n\rSector %d contents:\n\n\r",sectornum);
	sector = floppydisk_read_sector (sectornum);



	if(sector!=0){
		int i = 0,c = 0,j = 0;
		for (c=0;c<4;c++){
			for(j=0;j<128;j++)
				kernelPrintf("0x%x ", sector[i+j]);
			i += 128;
			kernelPuts("\n\rPress any key to continue..\n\r");
			getch();
		}
	}
	else
		kernelPuts("\n\r !!! Error reading from floppy disk !!!");

	kernelPuts("\n\r Done..");
}

void cmd_read_file(){
	char path[32];
	kernelPuts("\n\rPlease select file path > ");
	get_cmd(path,32);
	kernelPuts("\n");
	FILE file = volOpenFile(path);
	if(file.flags == FS_INVALID){
		kernelPuts("\n\rError: unable to open file");
		return;
	}
	if((file.flags & FS_DIR)==FS_DIR){
		kernelPuts("\n\rUnable to display directory contents");
		return;
	}
	while(file.eof != 1){
		unsigned char buf[512];
		volReadFile(&file,buf,512);
				
		
		kernelPrintf((const char*)buf);
		
		if(file.eof!=1){
			kernelPuts("\n\rPress a key to continue");
			getch();
			kernelPuts("\r");
		}
	}

	kernelPuts("\n\rEnd of file reached!");
}

int run_cmd(char* cmd_buf){
	if(strcmp(cmd_buf,"halt")==0){
		kernelPuts("\nSeaStar OS Halted. You can now shoutdown your computer");
		return 1;
	}

	else if(strcmp(cmd_buf,"floppydump")==0){
		cmd_read_sect();
	}

	else if(strcmp(cmd_buf,"cat")==0){
		cmd_read_file();
	}

	else if(strcmp(cmd_buf,"cls")==0){
		kernelClrScr(0x0f);
	}
	
	else if(strcmp (cmd_buf,"help") == 0) {
		kernelPuts("\nBenvenuto sull'help! Questa parte e' ancora da scrivere ^^");
	}
	
	else{
		kernelPrintf("\nUnknown command");
	}
	
	return 0;

}

void run(){
	char cmd_buf[100];

	while(1){
		cmd();

		get_cmd(cmd_buf,98);
	

		if(run_cmd(cmd_buf)==1)
			break;
	}
}










void kmain (multiboot_info_t* MultibootStructure)
{

	extern  uint32_t end; // Trick from bootstrap / linking script to get the kernel end addr

	
	
	uint32_t kernelSize=(int)&end;
	kernelSize -= 0x100000; // Real Kernel Size
	



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

	floppydisk_set_working_drive (0);
	floppydisk_install(38);
	floppydisk_set_dma(0x8000);
	kernelPrintf("Floppy Driver Installed.. \n");

	fsysFatInit();
	kernelPrintf("Virtual Filesystem Installed.. \n");
	kernelPrintf("FAT12 Filesystem Initialized.. \n");
	kernelPrintf("Running SeaShell.... \n");
	run();

	while(1);


}

