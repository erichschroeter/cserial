#include <stdio.h>
#include <string.h>
#include <time.h>

#include "cserial.h"

/* returns the difference in milliseconds */
static unsigned int diff(struct timespec *start, struct timespec *end)
{
	return ((end->tv_sec * 1000) + (end->tv_nsec / 1000000)) -
		((start->tv_sec * 1000) + (start->tv_nsec / 1000000));
}

static void print_timestamp(struct timespec *start, struct timespec *end)
{
	printf("%dms", diff(start, end));
}

int main(int argc, char **argv)
{
	struct cserial_port port;
	int ret, i;
#define SIZE 1
	char buf[SIZE];
	struct timespec now, rxtime;
	struct cserial_port_conf conf = {
		.baud = 19200,
		.parity = PARITY_NONE,
		.csize = 8,
		.stopbits = 1,
	};
	char *tty;

	tty = (argc > 1 && argv[1] != NULL) ? argv[1] : "/dev/ttyUSB0";

	if (ret = cserial_open(&port, &conf, tty))
		fprintf(stderr, "cserial_open Error %d: %s\n", ret, strerror(ret));

	clock_gettime(CLOCK_MONOTONIC, &rxtime);

	while ((ret = cserial_read(&port, buf, SIZE)) != -1) {
		clock_gettime(CLOCK_MONOTONIC, &now);
		print_timestamp(&rxtime, &now);
		printf(": ");
		for (i = 0; i < ret; i++) {
			if (i != 0)
				printf(" ");
			printf("%02x", buf[i]);
		}
		printf("\n");
		clock_gettime(CLOCK_MONOTONIC, &rxtime);
	}
	if (ret)
		fprintf(stderr, "cserial_read Error %d: %s\n", ret, strerror(ret));

	if (ret = cserial_close(&port))
		fprintf(stderr, "cserial_close Error %d: %s\n", ret, strerror(ret));

	return 0;
}
