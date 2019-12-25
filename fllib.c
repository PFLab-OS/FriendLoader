#include "fllib.h"

extern int main();

/* DEPLOY_PHYS_ADDR_START + 1M (for img) */
static char *malloc_ptr = (char *)0xba700000 + 0x100000;
/* DEPLOY_PHYS_ADDR_END - 1M (for stack) */
static char * const malloc_end = (char *)0xbf6bffff - 0x100000;

static int *flbuf = (int *)FRIEND_LOADER_BUF + 1;

void exit(int status)
{
	int *flbuf = (int *)FRIEND_LOADER_BUF;
	flbuf[0] = status;
	while(1) {
		__asm__("wfe");
	}
}

void *malloc(unsigned long size)
{
	void *ptr = malloc_ptr;
	malloc_ptr += size;
	if (malloc_ptr > malloc_end) { exit(-1); }
	return ptr;
}

void flbuf_put(int x)
{
	*flbuf = x;
	flbuf++;
	if (flbuf == (int *)FRIEND_LOADER_BUF_END)
		flbuf = (int *)FRIEND_LOADER_BUF + 1;
}


/* stub */
long time()
{
	return 0;
}

char *strcpy(char *s1, const char *s2)
{
	int i;
	for (i = 0; s2[i] != '\0'; ++i) {
		s1[i] = s2[i];
	}
	s1[i] = s2[i];
	return s1;
}

int strcmp(const char *s1, const char *s2)
{
	int i;
	for (i = 0; ; ++i) {
		if (s1[i] == s2[i]) {
			if (s1[i] == '\0') {
				return 0;
			}
		} else if (s1[i] > s2[i]) {
			return 1;
		} else {
			return -1;
		}
	}
}
