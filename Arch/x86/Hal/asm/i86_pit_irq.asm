[BITS 32]
[GLOBAL _i86_pit_irq]                  
[EXTERN _i86_pit_irq_c]  
;align 4

_i86_pit_irq:
    cli
    pushad
    call      _i86_pit_irq_c  
    popad
    sti
    iretd



