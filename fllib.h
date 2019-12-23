#pragma once

#define FRIEND_LOADER_BUF	0x1177000

/*
 * about malloc argument:
 * this hikey's size_t is "long unsigned int"
 * it can be confirmed by
 * cat /usr/lib/gcc/aarch64-linux-gnu/6/include/stddef.h | grep __SIZE_TYPE__
 */
void exit(int status);
void *malloc(unsigned long size);
char *strcpy(char *s1, const char *s2);
int strcmp(const char *s1, const char *s2);
