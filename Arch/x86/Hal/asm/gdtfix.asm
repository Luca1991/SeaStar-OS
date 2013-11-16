[BITS 32]
[GLOBAL gdt_fix]                  

gdt_fix:
   mov ax, 0x10
   mov ds, ax
   mov es, ax
   mov ss, ax
   mov fs, ax
   mov gs, ax
   jmp 8:flush_cpu
   flush_cpu:

   ret
