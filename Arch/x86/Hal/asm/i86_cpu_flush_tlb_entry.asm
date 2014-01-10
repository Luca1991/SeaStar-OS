[BITS 32]
[GLOBAL i86_cpu_flush_tlb_entry]


i86_cpu_flush_tlb_entry:
    mov eax, [esp+4]
    cli
    invlpg [eax]
    sti
    ret
