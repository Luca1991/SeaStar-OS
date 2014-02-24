[BITS 32]
[GLOBAL i86_floppy_irq]                  
[EXTERN i86_floppy_irq_c]  
align 4

i86_floppy_irq:
    cli
    pushad
    call     i86_floppy_irq_c  
    popad
    sti
    iretd



