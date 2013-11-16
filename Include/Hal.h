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

extern void setvect (int intno, void (far *vect)() ); // Sets new interrupt vector


extern const char* get_cpu_vendor(); // Returns the cpu vendor

extern int get_tick_count(); // Returns current tick count

#endif
