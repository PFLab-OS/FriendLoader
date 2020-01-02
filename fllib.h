#pragma once

/*
 * about malloc argument:
 * this hikey's size_t is "long unsigned int"
 * it can be confirmed by
 * cat /usr/lib/gcc/aarch64-linux-gnu/6/include/stddef.h | grep __SIZE_TYPE__
 */
void exit(int status);
void flbuf_put(int x);
int fork(void);
void *malloc(unsigned long size);
unsigned long read(int fd, void *buf, unsigned long count);
char *strcpy(char *s1, const char *s2);
int strcmp(const char *s1, const char *s2);
long time(long* timer);
unsigned long write(int fd, const void *buf, unsigned long count);
