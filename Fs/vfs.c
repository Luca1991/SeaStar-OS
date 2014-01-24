#include <fs/vfs.h>
#include <string.h>


#define MAX_DEVICES 26 // Max number of mounted devices

PFILESYSTEM _FileSystems[MAX_DEVICES]; // Fs ist

// Open file - fname = full path, device = device ID
FILE volOpenFile(const char* fname,unsigned char device){
	if(fname){
	//	unsigned char device = 'a'; // Default device 'a'
		char* filename = (char*) fname;
		// FIXME this should be fname[1], but due to shift bug we have to check fname[2]
		if(fname[1]==':'){
			device = fname[0];
			filename += 2; // FIXME 2 or 3 ??
		}

		if(_FileSystems[device - 'a']){ // FS Call
			FILE file = _FileSystems[device-'a']->Open(filename);
			file.deviceID = device;
			return file;
		}
	}

	FILE file; // if we can't call fs, then return an invalid file
	file.flags = FS_INVALID;
	return file;

}

// Read file
void volReadFile(PFILE file, unsigned char* buffer, unsigned int length){
	if(file)
		if(_FileSystems[file->deviceID - 'a'])
			_FileSystems[file->deviceID -'a']->Read(file,buffer,length);
}

// Close file
void volCloseFile(PFILE file){
	if(file)
		if(_FileSystems[file->deviceID - 'a'])
			_FileSystems[file->deviceID - 'a']->Close(file);
}

// Register a FS
void volRegisterFileSystem (PFILESYSTEM fsys, unsigned int deviceID){
	static int i=0; // FIXME this should be global..
	
	if(i<MAX_DEVICES)
		if(fsys){
			_FileSystems[deviceID] = fsys;
			i++;
		}
}

// Unregister a FS
void volUnregisterFileSystem (PFILESYSTEM fsys){
	int i = 0;	
	for(i=0;i<MAX_DEVICES;i++)
		if(_FileSystems[i]==fsys)
			_FileSystems[i]=0;
}

// Unregister a FS by ID
void volUnregisterFileSystemByID(unsigned int deviceID){
	if(deviceID < MAX_DEVICES)
		_FileSystems[deviceID] = 0;
}


