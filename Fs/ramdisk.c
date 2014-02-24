#include <string.h>
#include <fs/vfs.h>
#include <fs/ramdisk.h>


FILESYSTEM _FSysRd; // Ramdisk FS

unsigned int initrdAddr;

// Convert file size (from tar header) from octal to decimal
unsigned int getsize(const char *in){
    unsigned int size = 0;
    unsigned int j;
    unsigned int count = 1;

    for (j = 11; j > 0; j--, count *= 8)
        size += ((in[j - 1] - '0') * count);

    return size;
}

int check_subdir(char* filename){
	int k = 0;
	int isSub=0;
	for(k = 0;filename[k] != '\0'; k++){
		if((filename[k]=='/')&&(filename[k+1]!='\0')){
			isSub=1;
			break;		
		}
	}
	
	return isSub;	
}

void check_dir(char* filename){
	int k = 0;
	for(k = 0;filename[k] != '\0'; k++)
		if(filename[k]=='/'&&(filename[k+1]=='\0'))
			filename[k]='\0';
}



// Root directory reader
FILE fsysRdOpenRootDirectory(const char* DirectoryName){
	FILE file;
	int i = 0;
	unsigned int address = initrdAddr;	

	for (i = 0; ; i++){

        	struct tar_header *header = (struct tar_header *)address;

        	if (header->filename[0] == '\0')
           		break;

		unsigned int size = getsize(header->size);


		if(check_subdir((char*)header->filename)){   // If it is a file in a subdir, then ignore it (we are in the root dir here!)
			address += ((size / 512) + 1) * 512;

       			if (size % 512)
            			address += 512;		
			
			continue;			
		}
						
        	check_dir((char*)&header->filename); // If it is a dir, then remove the last '/' in the filename



		if(strcmp(DirectoryName,header->filename) == 0){

			strcpy(file.name, DirectoryName);
			file.id = 0;
			file.currentCluster = address+512; // TAR Header is 512 bytes, so our file start at address+512
			file.fileLength = size;
			file.eof = 0;
			file.position = address+512;
		
			if(header->linkflag == RD_DIR){
				file.flags = FS_DIR; // File is a directory
			}
			else
				file.flags = FS_FILE; // else, it is a file

			return file;
		}

      	  	address += ((size / 512) + 1) * 512;

       		if (size % 512)
            		address += 512;

    	}

	file.flags = FS_INVALID; // Unable to find file
	return file;
}
void get_filename(char* filename,const char* filepath){
	int k=0, i = 0;
	char fname[100];
	for(k=0;;k++){
		if(filepath[k]=='\0')
			break;

		if(filepath[k]=='/')
			i=0;
		else{
			fname[i]=filepath[k];
			i++;
		}			
	}
	fname[i]=0; // Null termitate
	strcpy(filename,fname);
} 

// Locate a file or folder in subdir
FILE fsysRdOpenSubDir(const char* filepath){

	char filename[100];
	get_filename((char*)&filename,filepath);

	FILE file;
	int i = 0;
	unsigned int address = initrdAddr;	
	for (i = 0; ; i++){

        	struct tar_header *header = (struct tar_header *)address;

        	if (header->filename[0] == '\0')
           		break;

		unsigned int size = getsize(header->size);

			
        	check_dir((char*)&header->filename); // If it is a dir, then remove the last '/' in the filename

		

		if(strcmp(filepath,header->filename) == 0){

			strcpy(file.name, filename);
			file.id = 0;
			file.currentCluster = address+512; // TAR Header is 512 bytes, so our file start at address+512
			file.fileLength = size;
			file.eof = 0;
			file.position = address+512;
		
			if(header->linkflag == RD_DIR){
				file.flags = FS_DIR; // File is a directory
			}
			else
				file.flags = FS_FILE; // else, it is a file

			return file;
		}

      	  	address += ((size / 512) + 1) * 512;

       		if (size % 512)
            		address += 512;

    	}

	file.flags = FS_INVALID; // Unable to find file
	return file;
}


// Open a file
FILE fsysRdOpen(const char* FileName){
	FILE curDirectory;
	char *p = 0;
	char* path = (char*) FileName;
	int rootDir = 1;

	p = strchr(path,'/'); // Check for '\' symbols in path
	if(!p){
		curDirectory = fsysRdOpenRootDirectory(path); //no, then search in root dir
		if(curDirectory.flags == FS_FILE) 
			return curDirectory;     // File found
		if(curDirectory.flags == FS_DIR) 
			return curDirectory;     // Dir found

		FILE ret;   
		ret.flags = FS_INVALID; // unable to find file
		return ret;            
	}

	p++; // Skip '/' and go to the next char
	
	while(p){
		char pathname[100];

		int i = 0;
		for(i=0;i<100;i++){  // get path name
			if(p[i]=='/' || p[i]=='\0') // if another '\' or end of line is reached, we are done
				break;

			pathname[i]=p[i]; // copy path
		}
		pathname[i] = 0; // null termitate

		if(rootDir==1){ 

			curDirectory = fsysRdOpenRootDirectory(pathname); // Search root directory
			rootDir = 0;
		}

		else{	

			curDirectory = fsysRdOpenSubDir(FileName+1); // Search a subdir instead passing the filepath without the first /
		}
		
		if(curDirectory.flags == FS_INVALID)
			break;		

		if(curDirectory.flags == FS_FILE)
			return curDirectory;  // File Found

		
		
		p=strchr(p+1,'/');
		if(p)
			p++;
	}

	if(curDirectory.flags == FS_DIR)
			return curDirectory; // Dir Found

	FILE ret;   
	ret.flags = FS_INVALID; // unable to find file
	return ret;  

}

// Close file
void fsysRdClose(PFILE file){
	if(file)
		file->flags = FS_INVALID;
}

// Read from file
void fsysRdRead(PFILE file, unsigned char* buffer, unsigned int length){
	if(file){
	
		memcpy(buffer,(unsigned char*)file->currentCluster,length);

		strcat((char*)buffer,'\0');
		
		uint32_t nextCluster = file->currentCluster+length; // Read entry for the next cluster

		if(nextCluster >= (file->position+file->fileLength)){	// check for EOF
			file->eof = 1;
			return;
		}
		if(nextCluster == 0){		// check for file corruption
			file->eof = 1;
			return;
		}
		
		file->currentCluster = nextCluster; // finally set the next cluster
	}
	
}

// Mount the filesystem
void fsysRdMount(unsigned char deviceID){
	volRegisterFileSystem(&_FSysRd,deviceID); // Register fs to volume manager
}

void fsysRdReadDirectory(const char* DirectoryName){
	// TODO: read dirent for 'ls' command	
	return;
}
// Init FS
void fsysRdInit(char *startAddress,unsigned char deviceID){

	initrdAddr = (uint32_t)startAddress;

	strcpy(_FSysRd.Name,"RAMDISK");
	_FSysRd.Directory = fsysRdReadDirectory;
	_FSysRd.Mount = fsysRdMount;
	_FSysRd.Open = fsysRdOpen;
	_FSysRd.Read = fsysRdRead;
	_FSysRd.Close = fsysRdClose;
	
	fsysRdMount(deviceID); // mount fs
}
