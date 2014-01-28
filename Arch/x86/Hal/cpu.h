#ifndef _CPU_H
#define _CPU_H

#include <stdint.h>
#include "regs.h"



extern int i86_cpu_initialize(); // Init x86 CPU

extern void i86_cpu_shutdown(); // Shutdown x86 CPU

extern char* i86_get_cpu_vendor(); // Get the CPU vender

extern void i86_cpu_flush_caches(); // This function is written is ASM

extern void i86_cpu_flush_caches_write(); // This function is written in ASM



#endif
