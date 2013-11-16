#ifndef _CPU_H_DEFINED
#define _CPU_H_DEFINED

#include <stdint.h>
#include "regs.h"



extern int i86_cpu_initialize(); // Init x86 CPU

extern void i86_cpu_shutdown(); // Shutdown x86 CPU

extern char* i86_get_cpu_vendor(); // Get the CPU vender

#endif
