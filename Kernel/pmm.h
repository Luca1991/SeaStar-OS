#ifndef _PMM_H
#define _PMM_H

#include <stdint.h>

typedef uint32_t physical_addr;

extern void pmm_init(size_t memSize, physical_addr bitmap);
extern void pmm_init_region(physical_addr base, size_t size);
extern void pmm_deinit_region(physical_addr base, size_t size);
extern void* pmm_alloc_block();
extern void pmm_free_block(void* p);
extern void* pmm_alloc_blocks(size_t size);
extern void pmm_free_blocks(void *p, size_t size);
extern size_t pmm_get_memory_size();
extern uint32_t pmm_get_used_block_count();
extern uint32_t pmm_get_free_block_count();
extern uint32_t pmm_get_block_count();
extern uint32_t pmm_get_block_size();
extern uint32_t pmm_is_paging();                
extern uint32_t pmm_is_paging_asm();            // This function is written in ASM 

extern uint8_t pmm_paging_enable(uint8_t a);       // This function is written in ASM
extern void pmm_load_PDBR(physical_addr addr);    // This function is written in ASM
extern physical_addr pmm_get_PDBR();		  // This function is written in ASM

#endif

