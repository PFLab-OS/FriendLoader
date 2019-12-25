#pragma once

#define FRIEND_LOADER_BUF	0x1177000
#define FRIEND_LOADER_BUF_END	0x1178000

/*
 * about malloc argument:
 * this hikey's size_t is "long unsigned int"
 * it can be confirmed by
 * cat /usr/lib/gcc/aarch64-linux-gnu/6/include/stddef.h | grep __SIZE_TYPE__
 */
void exit(int status);
void flbuf_put(int x);
void *malloc(unsigned long size);
long time();
char *strcpy(char *s1, const char *s2);
int strcmp(const char *s1, const char *s2);
