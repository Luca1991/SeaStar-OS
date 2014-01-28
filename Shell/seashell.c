#include <Hal.h>
#include <shell/seashell.h>
#include <rtc.h>
#include <string.h>
#include <drivers/keyboard_driver.h>
#include <drivers/floppy_driver.h>
#include <fs/fat12.h>
#include <KernelDisplay.h>
#include <stdio.h>
#include <fs/vfs.h>



char currPath[500];


KEYCODE getch(){
	KEYCODE key = KEY_UNKNOWN;

	while(key==KEY_UNKNOWN)
		key = kkeyboard_get_last_key();

	kkeyboard_discard_last_key();
	return key;
}

void cmd(){
	kernelSetColor (0x1a);
	kernelPrintf("\nRoot@SeaStar#-%s>",currPath);
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
	char fpath[500];
	if(path[2]=='>'){ // FIXME: this should be [1] and ':'... need to fix keyboard driver....
		strcpy(fpath,path);
	}
	else{
		strcpy(fpath,currPath);
		strcat(fpath,path);
	}
	kernelPrintf("DEBUG: selected path=%s",fpath);
	FILE file = volOpenFile(fpath,currPath[0]); // Read the file using VFS

	
	if(file.flags == FS_INVALID){
		kernelPuts("\n\rError: unable to open file");
		return;
	}
	if(file.flags == FS_DIR){ 
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

void print_time_and_date(){
	unsigned char hour, minute, second, day, month;int year;
	read_rtc( &hour, &minute, &second, &day, &month, &year);
	kernelPrintf ("\n %d:%d:%d - %d/%d/%d",hour,minute,second,day,month,year);
}

void cmd_change_dir(){
	char path[32];
	char testPath[500];
	kernelPuts("\n\rPlease select dir > ");
	get_cmd(path,32);
	if(path[2]=='>'){ // FIXME: this should be [1] and ':'... need to fix keyboard driver....
		strcpy(testPath,path);
	}
	else{
		strcpy(testPath,currPath);
		strcat(testPath,path);
	}
	// Now check if the folder really exist ..
	kernelPrintf("\n\rDEBUG: selected path=%s",testPath);
	FILE file = volOpenFile(testPath,currPath[0]);
	if(file.flags == FS_DIR){ // .. and if it really exist, set the new path
		if(path[2]=='>'){ // FIXME: this should be [1] and ':'... need to fix keyboard driver....
			strcpy(currPath,path);
			strcat(currPath,"\\");
		}
		else{
			strcat(currPath,path);
			strcat(currPath,"\\");
		}
		
	}
	else // .. else if it doesn't exist, print an error
		kernelPuts("\n\rDirectory not found ");
	
}

void user_test(){
	int stack=0;

	 asm ("movl %%esp,%0"
		    : "=r"(stack));


	tss_set_stack(0x10,stack);
	
	enter_usermode(); // Enter usermode


	char* hello="\nHello usermode world!\n";
	

	asm("mov $0,%%eax\n"
	    "lea (%0),%%ebx\n"
	    "int $0x80"
	    ::"b"(hello)); // Call kernelPrintf usign syscall 0 :)

	
	while(1);
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

	else if(strcmp(cmd_buf,"cd")==0){
		cmd_change_dir();
	}

	else if(strcmp(cmd_buf,"cls")==0){
		kernelClrScr(0x0f);
	}
	
	else if(strcmp(cmd_buf,"time")==0){
		print_time_and_date();
	}
	
	else if(strcmp(cmd_buf,"usertest")==0){
		user_test();
	}

	else if(strcmp (cmd_buf,"help") == 0) {
		kernelPuts("\nBenvenuto sull'help! Questa parte e' ancora da scrivere ^^");
	}
	
	else{
		kernelPrintf("\nUnknown command");
	}
	
	return 0;

}


void SeaShell(){
	char cmd_buf[100];
	strcpy(currPath, "a:\\"); 
	while(1){
		cmd();
		get_cmd(cmd_buf,98);
		if(run_cmd(cmd_buf)==1)
			break;
	}
}
