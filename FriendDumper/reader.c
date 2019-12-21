#include <stdio.h>
#include <stdlib.h>

int main()
{
	FILE *fp = fopen("/dev/friend_dumper", "r");
	char d[1];
	fread(d, 1, 1, fp);
	return 0;
}
