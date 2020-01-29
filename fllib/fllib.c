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
 * kernel
 * QUEUE_SIZE is 1MB
 * QUEUE_SIZE should be less than int_max
 */
#define KERNEL_BASE_PADDR	0xbe700000	
#define NUM_OF_FD		8
#define QUEUE_SIZE		0x100000

#define USER_FLBUF_BASE	((int *)FRIEND_LOADER_BUF + 8)

extern int main();

static int * const flbuf_end = (int *)(FRIEND_LOADER_BUF + 0x800);

/* DEPLOY_PHYS_ADDR_START + 1M (for img) */
static char *malloc_ptr = (char *)PADDR + 0x100000;
/* DEPLOY_PHYS_ADDR_START + 31M (for stack) */
static char * const malloc_end = (char *)PADDR + (31 * 0x100000);

static int volatile *flbuf = USER_FLBUF_BASE;

static unsigned long time_cnt_start;
static unsigned long cntfrq_el0 = 1920000; /* 1.92MHz */

static int head[NUM_OF_FD];
static int tail[NUM_OF_FD];

struct kernel {
	volatile int tail[NUM_OF_FD];
	volatile int queue[NUM_OF_FD][QUEUE_SIZE];
};

static volatile struct kernel *kp;

void init_kernel(void) {
	kp = (volatile struct kernel *)KERNEL_BASE_PADDR;
	for (int i = 0; i < NUM_OF_FD; i++) {
		head[i] = 0;
		tail[i] = 0;
		kp->tail[i] = 0;
	}
}

void startup()
{
	asm volatile("isb");
	asm volatile("mrs %0, cntpct_el0" : "=r"(time_cnt_start));
	init_kernel();
	main();
	asm volatile("b exitsucc");
}

void exit(int status)
{
	int *flbuf = (int *)FRIEND_LOADER_BUF;
	flbuf[0] = status;
	asm volatile("b poweroff");
}

void flbuf_put(int x)
{
	*flbuf = x;
	flbuf++;
	if (flbuf == flbuf_end)
		flbuf = USER_FLBUF_BASE;
}

/*
time_t time(time_t *timer)
{
	(void)timer;
	unsigned long cnt;
	asm volatile("isb");
	asm volatile("mrs %0, cntpct_el0" : "=r"(cnt));
	return (time_t)((cnt - time_cnt_start) / cntfrq_el0);
}
*/

int clock_gettime(clockid_t clk_id, struct timespec *sp)
{
	if (clk_id != CLOCK_MONOTONIC)
		return -1;

	unsigned long cnt;
	asm volatile("isb");
	asm volatile("mrs %0, cntpct_el0" : "=r"(cnt));
	
	unsigned long diff = cnt - time_cnt_start;
	sp->tv_sec = (time_t)(diff / cntfrq_el0);
	sp->tv_nsec = (long)((diff % cntfrq_el0) * 1000000000 / cntfrq_el0);

	return 0;
}

pid_t fork()
{
	return CPID;
}

void *malloc(unsigned long size)
{
	void *ptr = malloc_ptr;
	malloc_ptr += size;
	if (malloc_ptr > malloc_end) { exit(9); }
	return ptr;
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

char *strcpy(char *s1, const char *s2)
{
	int i;
	for (i = 0; s2[i] != '\0'; ++i) {
		s1[i] = s2[i];
	}
	s1[i] = s2[i];
	return s1;
}

size_t strlen(const char *s)
{
	size_t i;
	for (i = 0; ; i++) {
		if (s[i] == '\0')
			return i;
	}
}

ssize_t read(int fd, void *buf, size_t count) {
	if (count != 4) {
		exit(0x3);
	}

	/*
	flbuf_put(0x100);
	flbuf_put(fd);
	flbuf_put(head[fd]);
	flbuf_put(kp->tail[fd]);
	*/
	
	while (1) {
		if (head[fd] != kp->tail[fd])
			break;
	}

	/*
	flbuf_put(0x100);
	flbuf_put(fd);
	flbuf_put(head[fd]);
	flbuf_put(kp->tail[fd]);
	*/

	int i = kp->queue[fd][head[fd]];
	((int *)buf)[0] = i;

	head[fd] = (head[fd] + 1) % QUEUE_SIZE;

	return (ssize_t)count;
}

ssize_t write(int fd, const void *buf, size_t count) {
	if (count != 4) {
		exit(0x3);
	}

	/*
	flbuf_put(0x200);
	flbuf_put(fd);
	flbuf_put(tail[fd]);
	flbuf_put(kp->tail[fd]);
	*/

	int i = ((const int *)buf)[0];
	kp->queue[fd][tail[fd]] = i;

	asm volatile("dsb sy");

	tail[fd] = (tail[fd] + 1) % QUEUE_SIZE;
	kp->tail[fd] = tail[fd];

	return (ssize_t)count;
}
