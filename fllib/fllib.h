#pragma once

/* 
 * this can be confirmed by small c program (printf("%d\n", CLOCK_MONOTONIC))
 */
#define CLOCK_MONOTONIC	1

/* they are general or assumed value, I have not comfirmed yet */
typedef int pid_t;
typedef long time_t;
typedef int clockid_t;
struct timespec {
	time_t	tv_sec;
	long	tv_nsec;
};

/*
 * following definitions can be confirmed by
 * cat /usr/lib/gcc/aarch64-linux-gnu/6/include/stddef.h | grep __SIZE_TYPE__
 * cat /usr/lib/gcc/aarch64-linux-gnu/6/include/stddef.h | grep ssize_t
 */
typedef long unsigned int size_t; 
typedef long ssize_t;

void exit(int status);

void flbuf_put(int x);

int clock_gettime(clockid_t clk_id, struct timespec *tp);

pid_t fork(void);

void *malloc(size_t size);

/*
void *memcpy(void *buf1, const void *buf2, size_t n);
*/

char *strcpy(char *s1, const char *s2);
int strcmp(const char *s1, const char *s2);
/*
size_t strlen(const char *s);
*/

ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
