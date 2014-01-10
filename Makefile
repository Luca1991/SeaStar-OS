CC = i586-elf-gcc
LD = i586-elf-ld
ODIR = Build
IDIR = Include
CFLAGS = -Wall -O -fstrength-reduce -fomit-frame-pointer -nostdinc -fno-builtin -I./$(IDIR) -c -fno-strict-aliasing -fno-common -fno-stack-protector

        
all:
# Build C files
	$(CC) $(CFLAGS) -o $(ODIR)/main.o ./Kernel/main.c 
	$(CC) $(CFLAGS) -o $(ODIR)/string.o ./Lib/string.c 
	$(CC) $(CFLAGS) -o $(ODIR)/stdio.o ./Lib/stdio.c 
	$(CC) $(CFLAGS) -o $(ODIR)/KernelDisplay.o ./Kernel/KernelDisplay.c
	$(CC) $(CFLAGS) -o $(ODIR)/stdint.o ./Include/stdint.c 
	$(CC) $(CFLAGS) -o $(ODIR)/Hal.o ./Arch/x86/Hal/Hal.c
	$(CC) $(CFLAGS) -o $(ODIR)/cpu.o ./Arch/x86/Hal/cpu.c
	$(CC) $(CFLAGS) -o $(ODIR)/gdt.o ./Arch/x86/Hal/gdt.c
	$(CC) $(CFLAGS) -o $(ODIR)/idt.o ./Arch/x86/Hal/idt.c
	$(CC) $(CFLAGS) -o $(ODIR)/pic.o ./Arch/x86/Hal/pic.c
	$(CC) $(CFLAGS) -o $(ODIR)/pit.o ./Arch/x86/Hal/pit.c
	$(CC) $(CFLAGS) -o $(ODIR)/exception.o ./Kernel/exception.c
	$(CC) $(CFLAGS) -o $(ODIR)/panic.o ./Kernel/panic.c
	$(CC) $(CFLAGS) -o $(ODIR)/pmm.o ./Kernel/pmm.c
	$(CC) $(CFLAGS) -o $(ODIR)/vmm.o ./Kernel/vmm.c
	$(CC) $(CFLAGS) -o $(ODIR)/vmm_pde.o ./Kernel/vmm_pde.c
	$(CC) $(CFLAGS) -o $(ODIR)/vmm_pte.o ./Kernel/vmm_pte.c

# Build ASM files
	nasm -f elf ./Kernel/asm/bootstrap.asm -o $(ODIR)/bootstrap.o

	nasm -f elf ./Arch/x86/Hal/asm/i86_pit_irq.asm -o $(ODIR)/i86_pit_irq.o
	nasm -f elf ./Arch/x86/Hal/asm/gdtfix.asm -o $(ODIR)/gdtfix.o
	nasm -f elf ./Arch/x86/Hal/asm/i86_cpu_flush_caches.asm -o $(ODIR)/i86_cpu_flush_caches.o
	nasm -f elf ./Arch/x86/Hal/asm/i86_cpu_flush_caches_write.asm -o $(ODIR)/i86_cpu_flush_caches_write.o
	nasm -f elf ./Arch/x86/Hal/asm/i86_cpu_flush_tlb_entry.asm -o $(ODIR)/i86_cpu_flush_tlb_entry.o



	nasm -f elf ./Kernel/asm/pmm_paging_enable.asm -o $(ODIR)/pmm_paging_enable.o
	nasm -f elf ./Kernel/asm/pmm_is_paging_asm.asm -o $(ODIR)/pmm_is_paging_asm.asm.o
	nasm -f elf ./Kernel/asm/pmm_load_PDBR.asm -o $(ODIR)/pmm_load_PDBR.o
	nasm -f elf ./Kernel/asm/pmm_get_PDBR.asm -o $(ODIR)/pmm_get_PDBR.o


# Link 
	$(LD) -T linker.ld $(ODIR)/*.o

# Copy the kernel to iso building dir and create our ISO file
	cp kernel.bin A_isodir/boot
	grub-mkrescue -o SeaStar.iso A_isodir

clean:
	rm -f kernel.bin
	rm -f $(ODIR)/*.*
	rm -f A_isodir/boot/kernel.bin
	rm -f SeaStar.iso
                  
       
