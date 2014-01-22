#ifndef _BPB_H
#define _BPB_H

#include <stdint.h>

#define __packed

// BPB

struct _BIOSPARAMETERBLOCK{
	uint8_t OEMName[8];
	uint16_t BytesPerSector;
	uint8_t SectorsPerCluster;
	uint16_t ReservedSectors;
	uint8_t NumberOfFats;
	uint16_t NumDirEntries;
	uint16_t NumSectors;
	uint8_t Media;
	uint16_t SectorsPerFat;
	uint16_t SectorsPerTrack;
	uint16_t HeadsPerCyl;
	uint32_t HiddenSectors;
	uint32_t LongSectors;
}__attribute((packed));

typedef struct _BIOSPARAMETERBLOCK BIOSPARAMETERBLOCK, *PBIOSPARAMETERBLOCK;

// BPB Extended Attributes

struct _BIOSPARAMETERBLOCK_EXT{
	uint32_t SectorPerFat32;
	uint16_t Flags;
	uint16_t Version;
	uint32_t RootCluster;
	uint16_t InfoCluster;
	uint16_t BackupBoot;
	uint16_t Reserved[6];
}__attribute((packed));

typedef struct _BIOSPARAMETERBLOCK_EXT BIOSPARAMETERBLOCKEXT, *PBIOSPARAMETERBLOCKEXT;

// Boot Sector
struct _BOOTSECTOR{
	uint8_t ignored[3]; // first 3 bytes are ignored..
	BIOSPARAMETERBLOCK Bpb;
	BIOSPARAMETERBLOCKEXT BpbExt;
	uint8_t Filler[448]; // Filler used to make struct 512 bytes in size
}__attribute((packed));

typedef struct _BOOTSECTOR BOOTSECTOR, *PBOOTSECTOR;


#undef __packed

#endif
