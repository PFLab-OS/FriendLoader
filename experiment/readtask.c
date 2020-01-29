#ifdef FRIEND
#include "../fllib/fllib.h"
#else
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#endif

#ifdef FRIEND
#define flbuf_put(i) flbuf_put(i)
#else
#define flbuf_put(i)
#endif

#ifdef FRIEND
const long long N = 10000;	/* 1 * 10e+4 */
#else
const long long N = 100000;	/* 1 * 10e+5 */
#endif

int main()
{
	int pipebuf;

#ifdef FRIEND
	int from_p = 0, to_c = 0;
	int from_c = 1, to_p = 1;
#else
	int p_to_c[2], c_to_p[2];
	if (pipe(p_to_c) == -1 || pipe(c_to_p) == -1) {
		perror("pipe");
		return 1;
	}
	int from_p = p_to_c[0], to_c = p_to_c[1];
	int from_c = c_to_p[0], to_p = c_to_p[1];
#endif

	pid_t cpid = fork();
	if (cpid == -1)
		exit(3);

#ifndef FRIEND
	if (cpid != 0) {
		if (close(from_p) == -1 || close(to_p) == -1) {
			perror("close");
			return 1;
		}
	} else {
		if (close(from_c) == -1 || close(to_c) == -1) {
			perror("close");
			return 1;
		}
	}
#endif

	struct timespec st, en;
	if (clock_gettime(CLOCK_MONOTONIC, &st) == -1) {
		exit(3);
	}
	for (int i = 1; i <= N; ++i) {
		/*
		flbuf_put(0x300);
		flbuf_put(i);
		flbuf_put(0);
		flbuf_put(0);
		*/
		if (cpid != 0) {
			write(to_c, &i, sizeof(i));
			read(from_c, &pipebuf, sizeof(pipebuf));
			if (i != pipebuf)
				exit(4);
		} else {
			write(to_p, &i, sizeof(i));
			read(from_p, &pipebuf, sizeof(pipebuf));
			if (i != pipebuf)
				exit(4);
		}
	}
	if (clock_gettime(CLOCK_MONOTONIC, &en) == -1)
		exit(3);

#ifndef FRIEND
	if (cpid != 0)
		wait(NULL);
	else
		exit(EXIT_SUCCESS);
#endif

	double diff = (en.tv_sec - st.tv_sec) +
		      ((en.tv_nsec - st.tv_nsec) * 0.000000001);
#ifdef FRIEND
	flbuf_put((int)((double)N / diff));
#else
	printf("%f\n", (double)N / diff);
#endif

	return 0;
}


