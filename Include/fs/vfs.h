#ifndef _VFS_H
#define _VFS_H

#include <stdint.h>

// FILE

typedef struct _FILE{
	char name[32];
	uint32_t flags;
	uint32_t fileLength;
	uint32_t id;
	uint32_t eof;
	uint32_t position;
	uint32_t currentCluster;
	uint32_t deviceID;
}FILE, *PFILE;

// FS Interface
typedef struct _FILE_SYSTEM{
	char Name[8];
	FILE (*Directory) (const char* DirectoryName);
	void (*Mount) ();
	void (*Read) (PFILE file, unsigned char* buffer, unsigned int length);
	void (*Close) (PFILE);
	FILE (*Open) (const char* FileNane);
}FILESYSTEM, *PFILESYSTEM;

// File Flags

#define FS_FILE 0
#define FS_DIR 1
#define FS_INVALID 2

extern FILE volOpenFile(const char* fname,unsigned char device);
extern void volReadFile(PFILE file, unsigned char* buffer, unsigned int length);
extern void volCloseFile(PFILE file);
extern void volRegisterFileSystem(PFILESYSTEM, unsigned int deviceID);
extern void volUnregisterFileSystem(PFILESYSTEM);
extern void volUnregisterFileSystemByID(unsigned int deviceID);

#endif
