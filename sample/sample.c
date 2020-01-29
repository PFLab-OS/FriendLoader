#include "../fllib/fllib.h"

int main()
{
	/* entering main */
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	flbuf_put((int)ts.tv_nsec);

	/* library function call (malloc) and save & restore local variable */ 
	int a = 1; 
	int *ptr = malloc(sizeof(int));
	*ptr = a;
	flbuf_put(*ptr);

	/* strcmp */
	int b1 = strcmp("aaa", "aaa");
	int b2 = strcmp("abc", "aaa");
	int b3 = strcmp("aaa", "abc");
	if (b1 == 0 && b2 > 0 && b3 < 0) {
		flbuf_put(1);
	} else {
		flbuf_put(-1);
	}

	/* strcpy */
	char c1[] = "abc";
	char c2[] = "ddd";
	strcpy(c1, c2);
	if (strcmp(c1, c2) == 0) {
		flbuf_put(1);
	} else {
		flbuf_put(-1);
	}

	/* memcpy */
	/*
	char c3[] = "afjao;ejifakl";
	char c4[] = "jdafkjdos;ilj";
	memcpy(c3, c4, sizeof(c3));
	if (strcmp(c3, c4) == 0) {
		flbuf_put(1);
	} else {
		flbuf_put(-1);
	}
	*/

	char c5[] = "a;kdfjoijkfe;;aiwoejfkjaspi;djfldsafk";
	flbuf_put((int)strlen(c5));

	clock_gettime(CLOCK_MONOTONIC, &ts);
	flbuf_put((int)ts.tv_nsec);

	return 0;
}
