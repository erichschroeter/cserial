#include "serial_port.h"

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int cserial_init(struct serial_port *port)
{
	/* save current serial port settings */
	tcgetattr(port->fd, &port->oldtio);
	/* clear struct for new port settings */
	memset(&port->tio, 0, sizeof(port->tio));
	cfmakeraw(&port->tio);
	cfsetospeed(&port->tio, B115200);
	cfsetispeed(&port->tio, B115200);
	port->tio.c_cflag |= CS8 | CREAD | CLOCAL;
	port->tio.c_iflag &= ~(ICRNL | INLCR);
	port->tio.c_oflag &= ~(ONLCR);

	/* clean the modem line and activate the settings for the port */
	tcflush(port->fd, TCIFLUSH);
	tcsetattr(port->fd, TCSANOW, &port->tio);

	return 0;
}

int cserial_open(struct serial_port *port, char *device)
{
	int ret = 0;

	/*
	 * Open modem device for reading and writing and not as a controlling
	 * tty so we don't get killed if line noise sends Ctrl + C.
	 */
	port->fd = open(device, O_RDWR | O_NOCTTY);

	if (port->fd <= 0) { ret = errno; goto fail; }
	if (!isatty(port->fd)) { ret = ENOTTY; goto fail_tty; }

	/* Keep reference to device opened. */
	port->device = calloc(strlen(device) + 1, sizeof(char));
	if (!port->device) { ret = errno; goto fail_tty; }
	strncpy(port->device, device, strlen(device));

	ret = cserial_init(port);
	if (ret) { goto fail_tty; }

	return ret;
fail_tty:
	close(port->fd);
fail:
	return ret;
}

int cserial_close(struct serial_port *port)
{
	/* restore old port settings */
	tcsetattr(port->fd, TCSANOW, &port->oldtio);
	close(port->fd);
	return 0;
}

int cserial_read(struct serial_port *port, void *buf, int size)
{
	return 0;
}

int cserial_write(struct serial_port *port, const void *buf, int size)
{
	return 0;
}

