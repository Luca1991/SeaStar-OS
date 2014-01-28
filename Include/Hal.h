/* REMEMBER that Hal.c is platform dependent :) */
#ifndef _HAL_H
#define _HAL_H

#include <stdint.h>

#define interrupt
#define far
#define near

extern int hal_initialize(); // Init Hardware Abstraction Layer
extern int hal_shoutdown();  // Shutdown Hardware Abstraction Layer

extern void geninterrupt(int x); // Generate x Interrupt
extern void interruptdone(unsigned int intno); // Notifies to Hal that the interrupt is done

extern void sound(unsigned frequency);  // Play a sound using the Speaker

extern unsigned char inportb (unsigned short portid); // Read byte from device mapped at portid
extern void outportb (unsigned short portid, unsigned char value); // Write byte (value) to device mapped at portid

extern void enable_interrupt(); // Enable all Hardware interrupts
extern void disable_interrupt(); // Disable all Hardware interrupts

extern void setvect (int intno, void (far *vect)(),int flags); // Sets new interrupt vector


extern const char* get_cpu_vendor(); // Returns the cpu vendor

extern int get_tick_count(); // Returns current tick count

// sets the mode of a channel
extern void dma_set_mode (uint8_t channel, uint8_t mode);

// prepares for generic channel read
extern void dma_set_read (uint8_t channel);

// prepares for generic channel write
extern void dma_set_write (uint8_t channel);

// sets the address of a channel
extern void dma_set_address(uint8_t channel, uint8_t low, uint8_t high);

// sets the counter of a channel
extern void dma_set_count(uint8_t channel, uint8_t low, uint8_t high);

// masks a channel
extern void dma_mask_channel (uint8_t channel);

// unmasks a channel
extern void dma_unmask_channel (uint8_t channel);

// resets a flipflop
extern void dma_reset_flipflop (int dma);

// reset the dma to defaults
extern void dma_reset (int dma);

// sets an external page register
extern void dma_set_external_page_register (uint8_t reg, uint8_t val);

// unmasks all registers
extern void dma_unmask_all (int dma);

// sleep for ms
extern void sleep (int ms);

// Enter usermode function. 
extern void enter_usermode();

// Install Task State Segment (tss.c)
extern void install_tss(uint32_t idx, uint16_t kernelSS, uint16_t kernelESP);
// Init system calls
extern void syscall_init();

extern void tss_set_stack(uint16_t kernelSS, uint16_t kernelESP);

#endif
