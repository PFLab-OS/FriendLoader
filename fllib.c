#include "fllib.h"

#ifdef TASK1
#define PADDR			0xba700000
#define FRIEND_LOADER_BUF	0x01177000
#define CPID			1
#elif defined TASK2
#define PADDR 			0xbc700000
#define FRIEND_LOADER_BUF	0x01177800
#define CPID			0
#endif

/*
 * about kernel
 * QUEUE_SIZE is 1MB
 * QUEUE_SIZE should be less than int_max
 */
#define KERNEL_BASE_PADDR	0xbe700000	
#define NUM_OF_FD		8
#define QUEUE_SIZE		0x100000

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

struct kernel {
	volatile int head[NUM_OF_FD];
	volatile int tail[NUM_OF_FD];
	char queue[NUM_OF_FD][QUEUE_SIZE];
};

static struct kernel *kp;

void init_kernel() {
	kp = (struct kernel *)KERNEL_BASE_PADDR;
	for (int i = 0; i < NUM_OF_FD; i++) {
		kp->head[i] = 0;
		kp->tail[i] = 0;
	}
}

void startup()
{
	asm volatile("isb");
	asm volatile("mrs %0, cntpct_el0" : "=r"(time_cnt_start));
	init_kernel();
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

int fork()
{
	return CPID;
}

unsigned long read(int fd, void *buf, unsigned long count) {
	if (count == 0)
		return count;

	for (unsigned long i = 0; i < count; i++) {
		volatile int h, t;
		do {
			h = kp->head[fd];
			t = kp->tail[fd];
		} while (h == t);

		((char *)buf)[i] = kp->queue[fd][h];
		h = (h + 1) % QUEUE_SIZE;
		kp->head[fd] = h;
	}
	return count;
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

long time(long *timer)
{
	long cnt;
	asm volatile("isb");
	asm volatile("mrs %0, cntpct_el0" : "=r"(cnt));
	return (cnt - time_cnt_start) / cntfrq_el0;
}

unsigned long write(int fd, const void *buf, unsigned long count) {
	if (count == 0)
		return count;

	for (unsigned long i = 0; i < count; i++) {
		kp->queue[fd][kp->tail[fd]] = ((char *)buf)[i];
		kp->tail[fd] = (kp->tail[fd] + 1) % QUEUE_SIZE;
	}

	return count;
}

