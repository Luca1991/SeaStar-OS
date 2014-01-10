[BITS 32]
[GLOBAL pmm_load_PDBR]

pmm_load_PDBR:
    push ebp
    mov ebp,esp
    mov eax,[ebp+8] 
    mov cr3,eax
    pop ebp
    ret
