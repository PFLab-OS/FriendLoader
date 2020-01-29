#include <elf.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#define DEPLOY_PADDR1	0xba700000UL
#define DEPLOY_PADDR2	0xbc700000UL

const unsigned long imgsize_limit = 0x100000UL; /* 1MB */

const void *get_mmap_ptr(const char *filename)
{
	int fd = open(filename, O_RDONLY);
	if (fd == -1) {
		perror("open");
		return NULL;
	}
	
	struct stat filestat;
	if (fstat(fd, &filestat) == -1) {
		perror("fstat");
		return NULL;
	}
	size_t filesize = (size_t)filestat.st_size;
	
	const void *mmap_ptr = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE,
				    fd, 0);
	if (mmap_ptr == MAP_FAILED) {
		perror("mmap");
		return NULL;
	}
	
	return mmap_ptr;
}

bool is_valid_elf(const char *elfptr)
{
	const Elf64_Ehdr* elfhdr = (const Elf64_Ehdr *)elfptr;

	if (!strcmp((const char *)&elfhdr->e_ident[EI_MAG0], ELFMAG))
		return false;

	if (elfhdr->e_ident[EI_CLASS] != ELFCLASS64)
		return false;

	if (elfhdr->e_ident[EI_DATA] != ELFDATA2LSB)
		return false;

	if (elfhdr->e_ident[EI_VERSION] != EV_CURRENT)
		return false;

	if (elfhdr->e_ident[EI_OSABI] != ELFOSABI_SYSV)
		return false;
	
	if (elfhdr->e_machine != EM_AARCH64)
		return false;
	
	if (elfhdr->e_type != ET_EXEC)
		return false;

	return true;
}

int fwrite_pos(const void *ptr, unsigned long nbyte, FILE *stream,
	       unsigned long pos)
{
	if ((long)pos < 0) {
		fprintf(stderr, "pos too large\n");
		return -1;
	}
	
	if (fseek(stream, (long)pos, SEEK_SET) == -1) {
		perror("fseek");
		return -1;
	}
	
	if (fwrite(ptr, 1, nbyte, stream) < nbyte) {
		perror("fwrite");
		return -1;
	}
	
	return 0;
}

static int zerofill(FILE* file, unsigned long offset, size_t size)
{
	if ((long)offset < 0) {
		fprintf(stderr, "offset too large\n");
		return -1;
	}

	enum { BUF_SIZE = 4096, };
	static char zeros[BUF_SIZE] = {0};
	
	if (fseek(file, (long)offset, SEEK_SET) == -1) {
		perror("fseek");
		return -1;
	}
	
	unsigned long wsize;
	for (unsigned long wcnt = 0; wcnt < size; wcnt += wsize) {
		wsize = (size - wcnt < BUF_SIZE) ? size - wcnt : BUF_SIZE;
		if (fwrite(zeros, 1, wsize, file) < wsize) {
			perror("fwrite");
			return -1;
		}
	}
	
	return 0;
}

int relocate(const char *elfptr, FILE *imgfile, unsigned long deploy_paddr) {
	const Elf64_Ehdr *elfhdr = (const Elf64_Ehdr *)elfptr;
	const Elf64_Phdr *proghdrs =
		(const Elf64_Phdr *)(elfptr + elfhdr->e_phoff);	
	if (sizeof(*proghdrs) != elfhdr->e_phentsize) {
		fprintf(stderr, "proghdr size\n");
		return -1;
	}

	/*
	 * in hikey960, Elf64_Addr and Elf64_XWord is 8B (unsigned long)
	 * and size_t is also 8B (unsigned long)
	 */
	unsigned long imgsize = 0;
	unsigned long imgalign = sizeof(void *);
	for (int i = 0; i < elfhdr->e_phnum; i++) {
		if (proghdrs[i].p_vaddr < deploy_paddr)
		       continue;	
		unsigned long s = proghdrs[i].p_vaddr + proghdrs[i].p_memsz
				  - deploy_paddr;
		imgsize = (s > imgsize ? s : imgsize);
		unsigned long a = proghdrs[i].p_align;
		imgalign = (a > imgalign ? a : imgalign);
	}
	if (imgsize > imgsize_limit) {
		fprintf(stderr, "imgsize(0x%lx) too large (> 0x%lx)\n",
			imgsize, imgsize_limit);
		return -1;
	}
	
	printf("elf_reloc: starting relocation.\n"
	       "imgsize: %ld (0x%lx), align: %ld (0x%lx)\n",
	       imgsize, imgsize, imgalign, imgalign);
	
	for (int i = 0; i < elfhdr->e_phnum; i++) {
		if (proghdrs[i].p_type != PT_LOAD)
			continue;

		unsigned long vaddr = proghdrs[i].p_vaddr;
		if (vaddr < deploy_paddr) {
			printf("this section (0x%lx) is ignored\n", vaddr);
			continue;
		}
		vaddr -= deploy_paddr;

		printf("elf_reloc: relocating LOAD segment.\n"
		       "offset: 0x%lx, size: %lu (0x%lx), end: 0x%lx\n",
		       vaddr,
		       proghdrs[i].p_memsz, proghdrs[i].p_memsz,
		       proghdrs[i].p_vaddr + proghdrs[i].p_memsz);
			
		const char *segptr = elfptr + proghdrs[i].p_offset;
		unsigned long segsize = proghdrs[i].p_memsz;
		if (fwrite_pos(segptr, segsize, imgfile, vaddr) == -1)
			return -1;
		
		unsigned long padofs = vaddr + proghdrs[i].p_filesz;
		unsigned long padsize =
			proghdrs[i].p_memsz - proghdrs[i].p_filesz;
		if (zerofill(imgfile, padofs, padsize) == -1)
			return -1;
	}

	const Elf64_Shdr *secthdrs =
		(const Elf64_Shdr *)(elfptr + elfhdr->e_shoff);	
	if (sizeof(*secthdrs) != elfhdr->e_shentsize) {
		fprintf(stderr, "secthdr size\n");
		return -1;
	}
	
	/* Clear bss section */
	for (int i = 0; i < elfhdr->e_shnum; i++) {
		if (secthdrs[i].sh_type != SHT_NOBITS)
			continue;
		if ((secthdrs[i].sh_flags & SHF_ALLOC) != SHF_ALLOC)
			continue;

		unsigned long addr = secthdrs[i].sh_addr;
		if (addr < deploy_paddr) {
			printf("this section (0x%lx) is ignored\n", addr);
			continue;
		}
		addr -= deploy_paddr;

                printf("elf_reloc: zero-filling NOBITS section. "
                       "offset: 0x%lx, size: %zu (0x%lx), end: 0x%lx\n",
		       addr,
		       secthdrs[i].sh_size, secthdrs[i].sh_size,
		       addr + secthdrs[i].sh_size);
                if (zerofill(imgfile, addr, secthdrs[i].sh_size) == -1)
			return -1;
	}

	return 0;
}

int main(int argc, const char **argv)
{
	if (argc != 4) {
		fprintf(stderr, "Usage: %s <input> <output> <taskn>\n",
			argv[0]);
		return 1;
	}

	const char *elfptr = get_mmap_ptr(argv[1]);
	if (elfptr == NULL) {
		fprintf(stderr, "Failed to get mmap'ed file pointer\n");
		return 1;
	}
	
	const char *imgname = argv[2];
	FILE *imgfile = fopen(imgname, "w");
	if (imgfile == NULL) {
		perror("fopen");
		return 1;
	}

	if (!is_valid_elf(elfptr)) {
		fprintf(stderr, "this ELF is invalid\n");
		return 1;
	}

	unsigned long deploy_paddr;
	if (strcmp(argv[3], "1") == 0) {
		deploy_paddr = DEPLOY_PADDR1;
	} else if (strcmp(argv[3], "2") == 0) {
		deploy_paddr = DEPLOY_PADDR2;
	} else {
		fprintf(stderr, "argv[3] = %s\n", argv[3]);
		return 1;
	}

	if (relocate(elfptr, imgfile, deploy_paddr) == -1) {
		fprintf(stderr, "relocation failed\n");
		return 1;
	}

	printf("relocation success\n");
	return 0;
}
