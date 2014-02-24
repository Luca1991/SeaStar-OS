#include <KernelDisplay.h>
#include <elf32.h>

int elf_check_header(Elf32_Ehdr *hdr) {
	if(!hdr) return 0;
	if(hdr->e_ident[EI_MAG0] != ELFMAG0) {
		kernelPrintf("ELF Header EI_MAG0 incorrect.\n");
		return 0;
	}
	if(hdr->e_ident[EI_MAG1] != ELFMAG1) {
		kernelPrintf("ELF Header EI_MAG1 incorrect.\n");
		return 0;
	}
	if(hdr->e_ident[EI_MAG2] != ELFMAG2) {
		kernelPrintf("ELF Header EI_MAG2 incorrect.\n");
		return 0;
	}
	if(hdr->e_ident[EI_MAG3] != ELFMAG3) {
		kernelPrintf("ELF Header EI_MAG3 incorrect.\n");
		return 0;
	}
	return 1;
}

int elf_check_support(char* elf) {

	Elf32_Ehdr *hdr;

	hdr = (Elf32_Ehdr *) elf;
	
	if(!elf_check_header(hdr)) {
		kernelPrintf("Invalid ELF File.\n");
		return 0;
	}
	if(hdr->e_ident[EI_CLASS] != ELFCLASS32) {
		kernelPrintf("Unsupported ELF File Class.\n");
		return 0;
	}
	if(hdr->e_ident[EI_DATA] != ELFDATA2LSB) {
		kernelPrintf("Unsupported ELF File byte order.\n");
		return 0;
	}
	if(hdr->e_machine != EM_386) {
		kernelPrintf("Unsupported ELF File target.\n");
		return 0;
	}
	if(hdr->e_ident[EI_VERSION] != EV_CURRENT) {
		kernelPrintf("Unsupported ELF File version.\n");
		return 0;
	}
	if(hdr->e_type != ET_REL && hdr->e_type != ET_EXEC) {
		kernelPrintf("Unsupported ELF File type.\n");
		return 0;
	}
	return 1;
}


