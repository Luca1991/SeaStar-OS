[BITS 32]
[GLOBAL pmm_is_paging_asm]



pmm_is_paging_asm:
    mov eax,cr0
    ret
    
