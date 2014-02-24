[BITS 32]
[GLOBAL i86_keyboard_irq]                  
[EXTERN i86_keyboard_irq_c]  
align 4

i86_keyboard_irq:
    cli
    pushad
    call     i86_keyboard_irq_c  
    popad
    sti
    iretd



