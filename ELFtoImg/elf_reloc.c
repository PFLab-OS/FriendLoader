#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "elf_reloc.h"

static inline int is_elf(const Elf64_Ehdr* e_hdr);
static inline int is_elf64(const Elf64_Ehdr* e_hdr);
static inline int is_current_version(const Elf64_Ehdr* e_hdr);
static inline int is_abi_sysv(const Elf64_Ehdr* e_hdr);
static inline int is_machine_aarch64(const Elf64_Ehdr* e_hdr);

static const unsigned long deploy_phys_addr_start = 0xba700000UL;

int elf64_parse(const void* const elf_ptr, Elf64Exec* elf64_exec)
{
    const uint8_t* hdr = (const uint8_t*)elf_ptr;
    const Elf64_Ehdr* e_hdr = (const Elf64_Ehdr*)elf_ptr;

    // Validate ELF header
    const int is_valid_elf = is_elf(e_hdr)
                             && is_elf64(e_hdr)
                             && is_current_version(e_hdr)
                             && is_abi_sysv(e_hdr)
                             && is_machine_aarch64(e_hdr);
    if (!is_valid_elf)
        return -1;

    if (e_hdr->e_type != ET_EXEC)
        return -1;

    // Segments
    Elf64_Xword mem_size = 0;
    Elf64_Xword align = sizeof(void*);

    for (int i = 0; i < e_hdr->e_phnum; i++) {
        const Elf64_Phdr* prog_hdr
            = (const Elf64_Phdr*)(hdr + e_hdr->e_phoff + e_hdr->e_phentsize * i);

        Elf64_Xword ms = prog_hdr->p_vaddr + prog_hdr->p_memsz;
        if (mem_size < ms)
            mem_size = ms;
        if (align < prog_hdr->p_align)
            align = prog_hdr->p_align;
    }

    elf64_exec->header = hdr;
    elf64_exec->elf64_header = e_hdr;
    elf64_exec->mem_size = mem_size;
    elf64_exec->align = align;

    return 0;
}

static int fwrite_pos(
    FILE* file, unsigned long offset, const uint8_t* contents, size_t size);
static int ffill_zero(FILE* file, long offset, size_t size);

int elf64_reloc(const Elf64Exec* elf64_exec, FILE* img_file)
{
    printf("elf_reloc: starting relocation. "
           "mem_size: %ld (0x%lx), align: %ld (0x%lx)\n",
        elf64_exec->mem_size, elf64_exec->mem_size,
        elf64_exec->align, elf64_exec->align);

    const uint8_t* hdr = elf64_exec->header;
    const Elf64_Ehdr* e_hdr = elf64_exec->elf64_header;

    for (int i = 0; i < e_hdr->e_phnum; i++) {
        const Elf64_Phdr* prog_hdr
            = (const Elf64_Phdr*)(hdr + e_hdr->e_phoff + e_hdr->e_phentsize * i);

	unsigned long vaddr = (unsigned long)prog_hdr->p_vaddr;
        switch (prog_hdr->p_type) {
        case PT_LOAD:
		if (vaddr < deploy_phys_addr_start) {
			printf("this section (0x%lx) is ignored\n", vaddr);
			break;
		}
		vaddr -= deploy_phys_addr_start;
				    
            printf("elf_reloc: relocating LOAD segment. "
                   "offset: 0x%lx, size: %zu (0x%lx), end: 0x%lx\n",
                vaddr,
                prog_hdr->p_memsz, (long)prog_hdr->p_memsz,
                (long)(prog_hdr->p_vaddr + prog_hdr->p_memsz));
            if (fwrite_pos(
                    img_file,
                    vaddr,
                    &hdr[prog_hdr->p_offset],
                    prog_hdr->p_memsz)
                == -1)
                return -1;
            if (ffill_zero(
                    img_file,
                    (long)(vaddr + prog_hdr->p_filesz),
                    prog_hdr->p_memsz - prog_hdr->p_filesz)
                == -1)
                return -1;
            break;
        }
    }

    // Clear bss section
    for (int i = 0; i < e_hdr->e_shnum; i++) {
        const Elf64_Shdr* sct_hdr
            = (const Elf64_Shdr*)(hdr
                                  + e_hdr->e_shoff
                                  + e_hdr->e_shentsize * i);
	unsigned long addr = sct_hdr->sh_addr;
        if (sct_hdr->sh_type == SHT_NOBITS) {
            if ((sct_hdr->sh_flags & SHF_ALLOC) != 0) {
		    if (addr < deploy_phys_addr_start) {
			    printf("this section (0x%lx) is ignored\n", addr);
			    continue;
		    }
		    addr -= deploy_phys_addr_start;
                printf("elf_reloc: zero-filling NOBITS section. "
                       "offset: 0x%lx, size: %zu (0x%lx), end: 0x%lx\n",
                    addr,
                    sct_hdr->sh_size, (long)sct_hdr->sh_size,
                    (long)(addr + sct_hdr->sh_size));
                if (ffill_zero(
                        img_file,
                        addr,
                        sct_hdr->sh_size)
                    == -1)
                    return -1;
            }
        }
    }

    /*
    printf("elf_reloc: entry_point: 0x%lx\n", e_hdr->e_entry);
    */

    return 0;
}

#define EQ_ELF_IDENT(idx, val) e_hdr->e_ident[(idx)] == (val)

static inline int is_elf(const Elf64_Ehdr* e_hdr)
{
    return EQ_ELF_IDENT(0, ELFMAG0)
           && EQ_ELF_IDENT(1, ELFMAG1)
           && EQ_ELF_IDENT(2, ELFMAG2)
           && EQ_ELF_IDENT(3, ELFMAG3);
}

static inline int is_current_version(const Elf64_Ehdr* e_hdr)
{
    return EQ_ELF_IDENT(EI_VERSION, EV_CURRENT);
}

static inline int is_elf64(const Elf64_Ehdr* e_hdr)
{
    return EQ_ELF_IDENT(EI_CLASS, ELFCLASS64);
}

static inline int is_abi_sysv(const Elf64_Ehdr* e_hdr)
{
    return EQ_ELF_IDENT(EI_OSABI, ELFOSABI_SYSV);
}

static inline int is_machine_aarch64(const Elf64_Ehdr* e_hdr)
{
    return e_hdr->e_machine == EM_AARCH64;
}

static int fwrite_pos(
    FILE* file, unsigned long offset, const uint8_t* contents, size_t size)
{
    if (fseek(file, offset, SEEK_SET) == -1) {
        perror("fseek");
        return -1;
    }

    if (fwrite(contents, 1, size, file) < size) {
        perror("fwrite");
        return -1;
    }

    return 0;
}

static int ffill_zero(FILE* file, long offset, size_t size)
{
    enum { BUF_SIZE = 4096,
    };
    static uint8_t buf[BUF_SIZE] = {0};

    if (fseek(file, offset, SEEK_SET) == -1) {
        perror("fseek");
        return -1;
    }

    for (size_t written = 0; written < size; written += BUF_SIZE) {
        size_t write_size = (size - written < BUF_SIZE)
                                ? (size - written)
                                : BUF_SIZE;
        if (fwrite(buf, 1, write_size, file) < write_size) {
            perror("fwrite");
            return -1;
        }
    }

    return 0;
}
