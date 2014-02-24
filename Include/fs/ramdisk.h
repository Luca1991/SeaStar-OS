#ifndef _RAMDISK_H
#define _RAMDISK_H

#include <fs/vfs.h>

/*
SeaStar OS RAMDISK (Initrd):
The supported ramdisk format is a TAR archive.
RamDisk is a read-only FS.
*/


struct tar_header{
    char filename[100]; //ATTENTION: This contain the whole path, and not just the filename !
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];  //ATTENTION: Size is in octal base !
    char mtime[12];
    char chksum[8];
    char linkflag;
}__attribute((packed));

// Linkflag defines the type of the file
#define  RD_NORMAL    '0'        /* Normal disk file */
#define  RD_LINK      '1'        /* Link to previously dumped file */
#define  RD_DIR       '5'        /* Directory */

extern void fsysRdInit(char *startAddress,unsigned char deviceID);

#endif
