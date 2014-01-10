[BITS 32]
[GLOBAL pmm_paging_enable]                  


pmm_paging_enable:
    push ebp
    mov ebp,esp ;stack pointer to ebp
    mov ebx,[ebp+8] ;fetch first parameter
    mov eax,cr0
    cmp ebx,1                      
    je enable
    jmp disable
enable:
    or eax, 0x80000000
    mov cr0,eax
    jmp done
disable:
    and eax, 0x7FFFFFFF
    mov cr0,eax
done:
    pop ebp
    ret
