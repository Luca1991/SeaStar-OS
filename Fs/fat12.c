#include <fs/fat12.h>
#include <drivers/floppy_driver.h>
#include <bpb.h>
#include <string.h>
#include <ctype.h>
#include <fs/vfs.h>



#define SECTOR_SIZE 512 // 512 bytes per sectors

FILESYSTEM _FSysFat; // Fat FS
MOUNTINFO _MountInfo; 
uint8_t FAT[SECTOR_SIZE*2]; // This is needed because we need to read 2 sector to prevent that the last entry will be corrupted


// This function convert filename to fat12 format (8 char name, 3 char ext)
void ToDosFileName(const char* filename, char* fname, unsigned int FNameLength){
	unsigned int i = 0;
	if(FNameLength > 11)
		return ;
	if(!fname || !filename)
		return ;

	memset(fname,' ',FNameLength);

	for(i=0;i<strlen(filename) && i<FNameLength; i++){
		if(filename[i] == '.' || i==8)
			break; 
	


		fname[i] = toupper(filename[i]); // Filename in upper format
		
		

	}

	if(filename[i]=='.'){
		int k = 0;		
		for(k=0;k<3;k++){ 
			i++;
			if(filename[i])
				fname[8+k]=filename[i]; // Extension		
		}
		
	}

	for(i=0;i<3;i++)
		fname[8+i] = toupper(fname[8+i]); // Filename + Extension in upper format
				
	
}

// Root directory reader
FILE fsysFatDirectory(const char* DirectoryName){
	FILE file;
	unsigned char* buf;
	PDIRECTORY directory;
	int sector=0;
	int i = 0;

	char DosFileName[12];
	
	ToDosFileName(DirectoryName,DosFileName,11);
	
	DosFileName[11]=0;
	
	
	for(sector=0;sector<14;sector++){ // There are 14 sectors per directory
		buf = (unsigned char*) floppydisk_read_sector(_MountInfo.rootOffset+sector);
		directory = (PDIRECTORY) buf;
		for(i=0;i<16;i++){ // There are 16 entries per sector
			char name[12];
			memcpy(name,directory->Filename,11);
			name[11]=0;

			if(strcmp(DosFileName,name) == 0){
				
				strcpy(file.name, DirectoryName);
				file.id = 0;
				file.currentCluster = directory->FirstCluster;
				file.fileLength = directory->FileSize;
				file.eof = 0;
				file.fileLength = directory->FileSize;
				
			

				if(directory->Attribute == 0x10)
					file.flags = FS_DIR; // File is a directory
				else
					file.flags = FS_FILE; // else, it is a file

				return file;
				
			}
			directory++;		// next directory!
		}
	}
	file.flags = FS_INVALID; // Unable to find file
	return file;
}

// Read from file
void fsysFatRead(PFILE file, unsigned char* buffer, unsigned int length){
	if(file){
		unsigned int physSector = 32+(file->currentCluster-1); // Starting phys sector
		unsigned char* sector = (unsigned char*) floppydisk_read_sector(physSector); // Read the sector
		memcpy(buffer,sector,512);
		
		unsigned int FAT_Offset = file->currentCluster+(file->currentCluster/2); // locate the fact sector (1.5 because fat12 uses 8bits+4bits)
		unsigned int FAT_Sector = 1 + (FAT_Offset / SECTOR_SIZE);
		unsigned int entryOffset = FAT_Offset % SECTOR_SIZE;
		// Read 1st sector		
		sector = (unsigned char*) floppydisk_read_sector(FAT_Sector);
		memcpy(FAT,sector,512);
		// Read 2nd sector
		sector = (unsigned char*) floppydisk_read_sector(FAT_Sector+1);
		memcpy(FAT+SECTOR_SIZE,sector,512);
		
		uint16_t nextCluster = *(uint16_t*) &FAT[entryOffset]; // Read entry for the next cluster

		if(file->currentCluster & 0x0001) // check if entry is odd or even
			nextCluster >>= 4;	// grab high 12 bits
		else
			nextCluster &= 0x0FFF;	// grab low 12 bits

		if(nextCluster >= 0xff8){	// check for EOF
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

// Close file
void fsysFatClose(PFILE file){
	if(file)
		file->flags = FS_INVALID;
}

// Locate a file or folder in subdir
FILE fsysFatOpenSubDir(FILE kFile, const char* filename){	
	FILE file;
	int sector=0;
	unsigned int i = 0;
	char DosFileName[12];
	ToDosFileName(filename,DosFileName,11);
	DosFileName[11]=0;	
	//while(!kFile.eof){ // Read directory
	for(sector=0;sector<14;sector++){
		unsigned char buf[512];
		fsysFatRead(&file,buf,512);
		PDIRECTORY pkDir = (PDIRECTORY)buf; // set directory
		for(i=0;i<16;i++){  // 16 entries in buffer
			char name[12]; // get current filename
			memcpy(name,pkDir->Filename,11);
			name[11]=0;
			
			if(strcmp(name, DosFileName) == 0){ // found it!
				strcpy(file.name,filename);
				file.id = 0;
				file.currentCluster = pkDir->FirstCluster;
				file.fileLength = pkDir->FileSize;
				file.eof = 0;
				file.fileLength = pkDir->FileSize;

				if(pkDir->Attribute == 0x10)
					file.flags = FS_DIR; // File is a directory
				else
					file.flags = FS_FILE; 

				return file;			
			}
			
			pkDir++; // next dir
		}

	}
		
	file.flags = FS_INVALID;
	return file;
}

// Open a file
FILE fsysFatOpen(const char* FileName){
	FILE curDirectory;
	char *p = 0;
	int rootDir=1;
	char* path = (char*) FileName;

	p = strchr(path,'/'); // Check for '\' symbols in path
	if(!p){
		curDirectory = fsysFatDirectory(path); //no, then search in root dir
		if(curDirectory.flags == FS_FILE) 
			return curDirectory;     // File found
		if(curDirectory.flags == FS_DIR) 
			return curDirectory;     // Dir found

		FILE ret;   
		ret.flags = FS_INVALID; // unable to find file
		return ret;            
	}

	p++; // Skip '\' and go to the next char
	
	while(p){
		char pathname[16];
		int i = 0;
		for(i=0;i<16;i++){  // get path name
			if(p[i]=='/' || p[i]=='\0') // if another '\' or end of line is reached, we are done
				break;

			pathname[i]=p[i]; // copy path
		}
		pathname[i] = 0; // null termitate

		if(rootDir==1){ 
			curDirectory = fsysFatDirectory(pathname); // Search root directory
			rootDir = 0;
		}
		else{	
			
			curDirectory = fsysFatOpenSubDir(curDirectory,pathname); // Search a subdir instead
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

// Mount the filesystem
void fsysFatMount(unsigned char deviceID){
	
	volRegisterFileSystem(&_FSysFat,deviceID); // Register fs to volume manager

	PBOOTSECTOR bootsector; // boot sect infos
	
	bootsector = (PBOOTSECTOR) floppydisk_read_sector(0); // Read the boot sector
	
	_MountInfo.numSectors = bootsector->Bpb.NumSectors;
	
	_MountInfo.fatOffset = 1;
	_MountInfo.fatSize = bootsector->Bpb.SectorsPerFat;
	_MountInfo.fatEntrySize = 8;
	_MountInfo.numRootEntries = bootsector->Bpb.NumDirEntries;
	_MountInfo.rootOffset = (bootsector->Bpb.NumberOfFats * bootsector->Bpb.SectorsPerFat)+1;
	_MountInfo.rootSize = (bootsector->Bpb.NumDirEntries * 32 ) / bootsector->Bpb.BytesPerSector;
}
void fsysFatReadDirectory(const char* DirectoryName){
	// TODO: read dirent for 'ls' command	
	return;
}

// Init FS
void fsysFatInit(unsigned char deviceID){
	strcpy(_FSysFat.Name,"FAT12");
	_FSysFat.Directory = fsysFatReadDirectory;
	_FSysFat.Mount = fsysFatMount;
	_FSysFat.Open = fsysFatOpen;
	_FSysFat.Read = fsysFatRead;
	_FSysFat.Close = fsysFatClose;

	fsysFatMount(deviceID); // mount fs
	
}
