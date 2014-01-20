#ifndef _FLOPPY_DRIVER_H
#define _FLOPPY_DRIVER_H

#include <stdint.h>

void floppydisk_set_dma(int addr); 
void floppydisk_install(int irq);
void floppydisk_set_working_drive(uint8_t drive);
uint8_t floppydisk_get_working_drive();
uint8_t* floppydisk_read_sector (int secLBA);
void floppydisk_lba_to_chs (int lba, int *head, int *track, int *sector);

#endif
