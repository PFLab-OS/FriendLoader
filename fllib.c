#include "fllib.h"

#ifdef TASK1
#define PADDR			0xba700000
#define FRIEND_LOADER_BUF	0x01177000
#elif defined TASK2
#define PADDR 			0xbc700000
#define FRIEND_LOADER_BUF	0x01177800
#endif

#define USER_FLBUF_BASE	((int *)FRIEND_LOADER_BUF + 4)

extern int main();

static int * const flbuf_end = (int *)(FRIEND_LOADER_BUF + 0x800);

/* DEPLOY_PHYS_ADDR_START + 1M (for img) */
static char *malloc_ptr = (char *)PADDR + 0x100000;
/* DEPLOY_PHYS_ADDR_START + 31M (for stack) */
static char * const malloc_end = (char *)PADDR + (31 * 0x100000);

static int *flbuf = USER_FLBUF_BASE;

static unsigned long time_cnt_start;
static unsigned long cntfrq_el0 = 1920000; /* 1.92MHz */

void startup()
{
	asm volatile("isb");
	asm volatile("mrs %0, cntpct_el0" : "=r"(time_cnt_start));
	main();
}

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
	if (flbuf == flbuf_end)
		flbuf = USER_FLBUF_BASE;
}


/* return raw count */
long time(long *timer)
{
	long cnt;
	asm volatile("isb");
	asm volatile("mrs %0, cntpct_el0" : "=r"(cnt));
	return (cnt - time_cnt_start) / cntfrq_el0;
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
