[BITS 32]
[GLOBAL pmm_get_PDBR]

pmm_get_PDBR:
    mov eax,cr3
    ret
