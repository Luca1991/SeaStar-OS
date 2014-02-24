/* All these functions just call the kernel panic function */
#include "exception.h"
#include <Hal.h>

extern void  kernel_panic (const char* fmt, ...);



void divide_by_zero_fault (unsigned int cs, unsigned int eip, unsigned int eflags) {
	kernel_panic ("Divide by 0");
}


void single_step_trap (unsigned int cs, unsigned int eip, unsigned int eflags) {
	kernel_panic ("Single step");
}


void   nmi_trap (unsigned int cs, unsigned int eip, unsigned int eflags) {
	kernel_panic ("NMI trap");
}


void   breakpoint_trap (unsigned int cs,unsigned int eip, unsigned int eflags) {
	kernel_panic ("Breakpoint trap");
}


void   overflow_trap (unsigned int cs, unsigned int eip, unsigned int eflags) {
	kernel_panic ("Overflow trap");
}

void   bounds_check_fault (unsigned int cs, unsigned int eip, unsigned int eflags) {
	kernel_panic ("Bounds check fault");
}

void   invalid_opcode_fault (unsigned int cs, unsigned int eip, unsigned int eflags) {
	kernel_panic ("Invalid opcode");
}

void   no_device_fault (unsigned int cs, unsigned int eip, unsigned int eflags) {
	kernel_panic ("Device not found");
}

void   double_fault_abort (unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags) {
	kernel_panic ("Double Fault");
}

void   invalid_tss_fault (unsigned int cs,unsigned int err,  unsigned int eip, unsigned int eflags) {
	kernel_panic ("Invalid TSS");
}

void   no_segment_fault (unsigned int cs,unsigned int err,  unsigned int eip, unsigned int eflags) {
	kernel_panic ("Invalid segment");
}

void   stack_fault ( unsigned int cs,unsigned int err, unsigned int eip, unsigned int eflags) {
	kernel_panic ("Stack fault");
}

void   general_protection_fault (unsigned int cs,unsigned int err, unsigned int eip, unsigned int eflags) {
	kernel_panic ("General Protection Fault");
}

void   page_fault (unsigned int cs,unsigned int err, unsigned int eip, unsigned int eflags) {
	kernel_panic ("Page Fault");
}


void   fpu_fault (unsigned int cs, unsigned int eip, unsigned int eflags) {
	kernel_panic ("FPU Fault");
}

void   alignment_check_fault (unsigned int cs,unsigned int err, unsigned int eip, unsigned int eflags) {
	kernel_panic ("Alignment Check");
}

void   machine_check_abort (unsigned int cs, unsigned int eip, unsigned int eflags) {
	kernel_panic ("Machine Check");
}

void   simd_fpu_fault (unsigned int cs, unsigned int eip, unsigned int eflags) {
	kernel_panic ("FPU SIMD fault");
}

void   virtualization_exception (unsigned int cs, unsigned int eip, unsigned int eflags) {
	kernel_panic ("Virtualization Exception");
}

void   security_exception (unsigned int cs, unsigned int eip, unsigned int eflags) {
	kernel_panic ("Security Exception");
}
