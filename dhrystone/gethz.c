#include <stdio.h>
#include <unistd.h>

int main() {
	printf("%ld\n", sysconf(_SC_CLK_TCK));
	return 0;
}
