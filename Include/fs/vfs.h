#ifndef _VFS_H
#define _VFS_H

#include <stdint.h>

// FILE
typedef struct _FILE{
	char     name[32];
	uint32_t flags;
	uint32_t fileLength;
	uint32_t id;
	uint32_t eof;
	uint32_t position;
	uint32_t currentCluster;
	uint32_t deviceID;
}FILE, *PFILE;

// DIRECTORY ENTRIES
/*typedef struct _DIRENT{  //FIXME: dirent is not yet complete and stable. It will be disabled for now.
    char           name[32]; // Filename
    uint32_t       flags;    // File type (file or dir)
    uint32_t       size;     // If it is a file, store here the size  
}DIRENT, *PDIRENT;*/

// FS Interface
typedef struct _FILE_SYSTEM{
	char Name[8];
	void (*Directory) (const char* DirectoryName);
	void (*Mount) (unsigned char deviceID);
	void (*Read) (PFILE file, unsigned char* buffer, unsigned int length);
	void (*Close) (PFILE);
	FILE (*Open) (const char* FileNane);
}FILESYSTEM, *PFILESYSTEM;

// File Flags

#define FS_FILE 0
#define FS_DIR 1
#define FS_INVALID 2

extern FILE volOpenFile(const char* fname,unsigned char device);
extern void volReadDir(const char* dname,unsigned char device);
extern void volReadFile(PFILE file, unsigned char* buffer, unsigned int length);
extern void volCloseFile(PFILE file);
extern void volRegisterFileSystem(PFILESYSTEM, unsigned char deviceID);
extern void volUnregisterFileSystem(PFILESYSTEM); // This will unmount all FS of the specified type
extern void volUnregisterFileSystemByID(unsigned char deviceID); // This will unmount the fs of the specified device

#endif
