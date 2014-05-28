#ifndef __CSERIAL_H
#define __CSERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32

#include <windows.h>

#else /* UNIX */

#include <termios.h>
#include <signal.h>

#endif

#include <errno.h>

struct cserial_port {
#ifdef WIN32
	HANDLE fd;
	DCB oldDCB;
	COMMTIMEOUTS oldTimeouts;
	char *device;
#else /* UNIX */
	int fd;
	struct termios oldtio;
	struct termios tio;
	char *device;
#endif
};

struct cserial_port_conf {
	int baud;
#define PARITY_NONE 0
#define PARITY_ODD  1
#define PARITY_EVEN 2
	int parity;
	int csize;    /* character size */
	int stopbits;
	int flowcontrol_hw;
};

int cserial_open(struct cserial_port *port, struct cserial_port_conf *conf, char *device);
int cserial_init(struct cserial_port *port, struct cserial_port_conf *conf);
int cserial_close(struct cserial_port *port);
void cserial_free(struct cserial_port *port);
int cserial_read(struct cserial_port *port, void *buf, int size);
int cserial_write(struct cserial_port *port, const void *buf, int size);

#ifdef __cplusplus
}
#endif

#endif /* __CSERIAL_H */
