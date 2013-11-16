#!/bin/sh

build=Build
compiler=i586-elf-gcc
linker=i586-elf-ld
opt="-Wall -O -fstrength-reduce -fomit-frame-pointer -nostdinc -fno-builtin -I./Include -c -fno-strict-aliasing -fno-common -fno-stack-protector"

isodir=A_isodir
isoname=OS.iso

echo "Setting Path..."
echo "==========="
export PATH="/opt/cross/bin:$PATH"

echo "Removing Old Files..."
echo "==========="
rm -r ./Build/*
rm kernel.bin
rm $isoname
rm ./A_isodir/boot/kernel.bin

echo "Building..."
echo "==========="
# Build C files

$compiler $opt -o $build/main.o ./Kernel/main.c 
$compiler $opt -o $build/string.o ./Lib/string.c 
$compiler $opt -o $build/stdio.o ./Lib/stdio.c 
$compiler $opt -o $build/KernelDisplay.o ./Kernel/KernelDisplay.c
$compiler $opt -o $build/stdint.o ./Include/stdint.c 
$compiler $opt -o $build/Hal.o ./Arch/x86/Hal/Hal.c
$compiler $opt -o $build/cpu.o ./Arch/x86/Hal/cpu.c
$compiler $opt -o $build/gdt.o ./Arch/x86/Hal/gdt.c
$compiler $opt -o $build/idt.o ./Arch/x86/Hal/idt.c
$compiler $opt -o $build/pic.o ./Arch/x86/Hal/pic.c
$compiler $opt -o $build/pit.o ./Arch/x86/Hal/pit.c
$compiler $opt -o $build/exception.o ./Kernel/exception.c
$compiler $opt -o $build/panic.o ./Kernel/panic.c


# Build ASM files
nasm -f elf ./Kernel/asm/bootstrap.asm -o $build/bootstrap.o
nasm -f elf ./Arch/x86/Hal/asm/i86_pit_irq.asm -o $build/i86_pit_irq.o
nasm -f elf ./Arch/x86/Hal/asm/gdtfix.asm -o $build/gdtfix.o

# Link 
$linker -T linker.ld $build/*.o


# Copy the kernel to iso building dir and create our ISO file
cp kernel.bin $isodir/boot
grub-mkrescue -o $isoname $isodir

# Launch our OS on qemu or bochs
#qemu-system-i386 -cdrom $isoname
#bochs -log ./log.log -q

