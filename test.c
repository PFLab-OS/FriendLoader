#include "fllib.h"

int main()
{
	/* entering main */
	int *flbuf = (int *)FRIEND_LOADER_BUF;
	flbuf[1] = 1;

	/* library function call (malloc) and save & restore local variable */ 
	int a = 1; 
	int *ptr = malloc(sizeof(int));
	*ptr = a;
	flbuf[2] = *ptr;

	/* strcmp */
	int b1 = strcmp("aaa", "aaa");
	int b2 = strcmp("abc", "aaa");
	int b3 = strcmp("aaa", "abc");
	if (b1 == 0 && b2 > 0 && b3 < 0) {
		flbuf[3] = 1;
	} else {
		flbuf[3] = -1;
	}

	/* strcpy */
	char c1[] = "abc";
	char c2[] = "ddd";
	strcpy(c1, c2);
	if (strcmp(c1, c2) == 0) {
		flbuf[4] = 1;
	} else {
		flbuf[4] = -1;
	}

	return 0;
}
