[BITS 32]
[GLOBAL vmm_flush_tlb_entry]


vmm_flush_tlb_entry:
    mov eax, [esp+4]
    cli
    invlpg [eax]
    sti
    ret
