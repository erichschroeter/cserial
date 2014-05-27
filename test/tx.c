#include <stdio.h>
#include <string.h>

#include "cserial.h"

int main(int argc, char **argv)
{
	struct cserial_port port;
	int ret, i;
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

	for (i = 0; i < argc; i++) {
		if ((ret = cserial_write(&port, argv[i], strlen(argv[i]))) == -1)
			fprintf(stderr, "cserial_write Error %d: %s\n", ret, strerror(ret));
	}

	if (ret = cserial_close(&port))
		fprintf(stderr, "cserial_close Error %d: %s\n", ret, strerror(ret));

	return 0;
}
