#ifndef __CSERIAL_H
#define __CSERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <termios.h>
#include <signal.h>

#include <errno.h>

struct serial_port {
	int fd;
	struct termios oldtio;
	struct termios tio;
	char *device;
};

int cserial_open(struct serial_port *port, char *device);
int cserial_init(struct serial_port *port);
int cserial_close(struct serial_port *port);
int cserial_read(struct serial_port *port, void *buf, int size);
int cserial_write(struct serial_port *port, const void *buf, int size);

#ifdef __cplusplus
}
#endif

#endif /* __CSERIAL_H */
