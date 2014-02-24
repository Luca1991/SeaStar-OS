/* Here are defined our exception handlers */
#ifndef _EXCEPTION_H
#define _EXCEPTION_H

extern void  divide_by_zero_fault (unsigned int cs, unsigned int eip, unsigned int eflags);


extern void  single_step_trap (unsigned int cs, unsigned int eip, unsigned int eflags);


extern void  nmi_trap (unsigned int cs, unsigned int eip, unsigned int eflags);


extern void  breakpoint_trap (unsigned int cs, unsigned int eip, unsigned int eflags);


extern void  overflow_trap (unsigned int cs, unsigned int eip, unsigned int eflags);


extern void  bounds_check_fault (unsigned int cs, unsigned int eip, unsigned int eflags);


extern void  invalid_opcode_fault (unsigned int cs, unsigned int eip, unsigned int eflags);


extern void  no_device_fault (unsigned int cs, unsigned int eip, unsigned int eflags);


extern void  double_fault_abort (unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags);


extern void  invalid_tss_fault (unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags);


extern void  no_segment_fault (unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags);


extern void  stack_fault (unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags);


extern void  general_protection_fault (unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags);


extern void  page_fault (unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags);


extern void  fpu_fault (unsigned int cs, unsigned int eip, unsigned int eflags);


extern void  alignment_check_fault (unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags);


extern void  machine_check_abort (unsigned int cs, unsigned int eip, unsigned int eflags);


extern void  simd_fpu_fault (unsigned int cs, unsigned int eip, unsigned int eflags);


extern void  virtualization_exception (unsigned int cs, unsigned int eip, unsigned int eflags);


extern void  security_exception (unsigned int cs, unsigned int eip, unsigned int eflags);


#endif
