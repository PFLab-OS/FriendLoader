#include "fllib.h"

int main()
{
	/* entering main */
	flbuf_put(time((long *)0));

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

	flbuf_put(time((long *)0));

	return 0;
}
