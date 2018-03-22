CFLAGS = -Wall -Wextra -Wconversion

SRCS = main.c elf_reloc.c
HDRS = elf_reloc.h elfhead.h
PROD = main

.PHONY: clean

$(PROD): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) $(SRCS) -o $(PROD)

clean:
	rm -f $(PROD)
