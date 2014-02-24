#ifndef _ELF32_H
#define _ELF32_H

#include<stdint.h>
#include<fs/vfs.h>

typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Word;
typedef int32_t  Elf32_Sword;

// This is the ELF32 header structure according to official specifications.
typedef struct {
        unsigned char   e_ident[16];    // Mark the file as an object, and provide machine-independent data to decode and interpret the contents
        Elf32_Half      e_type;         // Object file type
        Elf32_Half      e_machine;      // Required arch
        Elf32_Word      e_version;      // Object file version
        Elf32_Addr      e_entry;        // Virtual addr of the entry point
        Elf32_Off       e_phoff;        // Program header table's file offset in bytes
        Elf32_Off       e_shoff;        // Section header table's file offset in bytes
        Elf32_Word      e_flags;        // CPU specific flags associated with the file
        Elf32_Half      e_ehsize;       // Elf32 header size in bytes
        Elf32_Half      e_phentsize;    // The size in bytes of one entry in the program header table
        Elf32_Half      e_phnum;        // The number of entries in the program header table
        Elf32_Half      e_shentsize;    // Section header's size in bytes 
        Elf32_Half      e_shnum;        // The number of entries in the section header table
        Elf32_Half      e_shtrndx;      // The section header table index of the entry associated with the section name string table
} Elf32_Ehdr;

#define EI_MAG0	        0	// ELF identification
#define EI_MAG1	        1	// ELF identification
#define EI_MAG2	        2	// ELF identification
#define EI_MAG3	        3	// ELF identification
#define EI_CLASS	4	// ELF arch (32/64)
#define EI_DATA	        5	// Data encoding
#define EI_VERSION	6	// File version
#define EI_OSABI	7	// OS/ABI identification
#define EI_ABIVERSION	8	// Version of ABI
#define EI_PAD	        9	// Start of padding bytes


// ELF32 Magic Number - EI_MAG
#define ELFMAG0	0x7F	
#define ELFMAG1	'E'	
#define ELFMAG2	'L'
#define ELFMAG3	'F'

// ELF32 CLASS - EI_CLASS
#define ELFCLASSNONE	0	// Class not valid
#define ELFCLASS32	1	// 32bit objects
#define ELFCLASS64	2	// 64bit objects

// ELF32 DATA - EI_DATA
#define ELFDATANONE	0	// Invalid data encoding
#define ELFDATA2LSB	1	// LSB first
#define ELFDATA2MSB	2	// MSB first

// Elf32_Ehdr->e_type
enum Elf_Type {
	ET_NONE		= 0, // Unkown Type
	ET_REL		= 1, // Relocatable File
	ET_EXEC		= 2  // Executable File
};

// Elf32_Ehdr->e_machine
#define EM_386		3       // x86 Machine Type

// ELF32 VERSION - EI_VERSION
#define	EV_CURRENT	1
#define	ELFVERSION	EV_CURRENT

// ELF32 Program header
typedef struct {
	Elf32_Word		p_type;
	Elf32_Off		p_offset;
	Elf32_Addr		p_vaddr;
	Elf32_Addr		p_paddr;
	Elf32_Word		p_filesz;
	Elf32_Word		p_memsz;
	Elf32_Word		p_flags;
	Elf32_Word		p_align;
} Elf32_Phdr;

// p_type
#define PT_NULL	        0
#define PT_LOAD	        1
#define PT_DYNAMIC	2
#define PT_INTERP	3
#define PT_NOTE	        4
#define PT_SHLIB	5
#define PT_PHDR	        6
#define PT_TLS	        7
#define PT_LOOS	        0x60000000
#define PT_HIOS	        0x6fffffff
#define PT_LOPROC	0x70000000
#define PT_HIPROC	0x7fffffff

// p_flags
#define PF_X	        0x1	        // Execute
#define PF_W	        0x2	        // Write
#define PF_R	        0x4	        // Read
#define PF_MASKOS	0x0ff00000	// Unspecified
#define PF_MASKPROC	0xf0000000	// Unspecified


// ELF32 Section Header
typedef struct {
	Elf32_Word	sh_name;
	Elf32_Word	sh_type;
	Elf32_Word	sh_flags;
	Elf32_Addr	sh_addr;
	Elf32_Off	sh_offset;
	Elf32_Word	sh_size;
	Elf32_Word	sh_link;
	Elf32_Word	sh_info;
	Elf32_Word	sh_addralign;
	Elf32_Word	sh_entsize;
} Elf32_Shdr;

# define SHN_UNDEF	(0x00) // Undefined/Not present

// sh_type
enum ShT_Types {
	SHT_NULL	= 0,   // Null section
	SHT_PROGBITS	= 1,   // Program information
	SHT_SYMTAB	= 2,   // Symbol table
	SHT_STRTAB	= 3,   // String table
	SHT_RELA	= 4,   // Relocation (w/ addend)
	SHT_NOBITS	= 8,   // Not present in file
	SHT_REL		= 9,   // Relocation (no addend)
};

// sh_flags
enum ShT_Attributes {
	SHF_WRITE	= 0x01, // Writable section
	SHF_ALLOC	= 0x02  // Exists in memory
};

// ELF32 Symbol table
typedef struct {
	Elf32_Word		st_name;
	Elf32_Addr		st_value;
	Elf32_Word		st_size;
	uint8_t			st_info;
	uint8_t			st_other;
	Elf32_Half		st_shndx;
} Elf32_Sym;

# define ELF32_ST_BIND(INFO)	((INFO) >> 4)
# define ELF32_ST_TYPE(INFO)	((INFO) & 0x0F)

// st_info
enum StT_Bindings {
	STB_LOCAL		= 0, // Local scope
	STB_GLOBAL		= 1, // Global scope
	STB_WEAK		= 2  // Weak, (ie. __attribute__((weak)))
};

// st_info
enum StT_Types {
	STT_NOTYPE		= 0, // No type
	STT_OBJECT		= 1, // Variables, arrays, etc.
	STT_FUNC		= 2  // Methods or functions
};

// ELF32 Relocation sections
typedef struct {
	Elf32_Addr		r_offset;
	Elf32_Word		r_info;
} Elf32_Rel;
 
typedef struct {
	Elf32_Addr		r_offset;
	Elf32_Word		r_info;
	Elf32_Sword		r_addend;
} Elf32_Rela;

# define ELF32_R_SYM(INFO)	((INFO) >> 8)
# define ELF32_R_TYPE(INFO)	((uint8_t)(INFO))

// r_info
enum RtT_Types {
	R_386_NONE		= 0, // No relocation
	R_386_32		= 1, // Symbol + Offset
	R_386_PC32		= 2  // Symbol + Offset - Section Offset
};


extern int elf_check_support(char* file);


#endif
