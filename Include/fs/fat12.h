#ifndef _FAT12_H
#define _FAT12_H

#include <stdint.h>
#include "vfs.h"

#define __packed
// Directory entry
struct _DIRECTORY{
	uint8_t Filename[8];
	uint8_t Extension[3];
	uint8_t Attribute;
	uint8_t	Reserved;
	uint8_t	TimeCreatedMS;
	uint16_t TimeCreated;
	uint16_t DateCreated;
	uint16_t DateLastAccessed;
	uint16_t FirstClusterHiBytes;
	uint16_t LastModTime;
	uint16_t LastModDate;
	uint16_t FirstCluster;
	uint32_t FileSize;	
}__attribute((packed));
typedef struct _DIRECTORY DIRECTORY, *PDIRECTORY;
#undef __packed

// FS mount infos
typedef struct _MOUNTINFO{
	uint32_t numSectors;
	uint32_t fatOffset;
	uint32_t numRootEntries;
	uint32_t rootOffset;
	uint32_t rootSize;
	uint32_t fatSize;
	uint32_t fatEntrySize;
}MOUNTINFO, *PMOUNTINFO;

extern FILE fsysFatDirectory (const char* DirectoryName);
extern void fsysFatRead(PFILE file, unsigned char* buffer, unsigned int length);
extern FILE fsysFatOpen(const char* filename);
extern void fsysFatInit();
extern void fsysFatMount();

#endif


