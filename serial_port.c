#include "serial_port.h"

#ifdef WIN32
#else /* UNIX */

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#endif

int cserial_init(struct serial_port *port)
{
#ifdef WIN32
	DCB conf;
	COMMTIMEOUTS timeouts;

	memcpy(&conf, &port->oldDCB, sizeof(DCB));

	conf.BaudRate = CBR_115200;
	conf.ByteSize = 8;
	conf.StopBits = ONESTOPBIT;
	conf.Parity = NOPARITY;

	if (!SetCommState(port->fd, &conf)) {
		/* failed to set the state of com port */
	}

	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if (!SetCommTimeouts(port->fd, &timeouts)) {
		/* failed to set the timeouts of com port */
	}

	return 0;
#else /* UNIX */
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
#endif
}

int cserial_open(struct serial_port *port, char *device)
{
	int ret = 0;
#ifdef WIN32
	LPCSTR _device = (LPCSTR) device;

	/* Keep reference to device opened. */
	port->device = calloc(strlen(_device) + 1, sizeof(char));
	if (!port->device) goto fail;
	strncpy(port->device, _device, strlen(_device));

	port->fd = CreateFile(_device, GENERIC_READ | GENERIC_WRITE,
		0, 0, OPEN_EXISTING, 0, 0);

	if (port->fd == INVALID_HANDLE_VALUE) {
		port->fd = NULL;
		goto fail_fd;
	}

	if (!GetCommState(port->fd, &port->oldDCB)) {
		/* could not get the state of com port */
		goto fail_comm_state;
	}

	if (!GetCommTimeouts(port->fd, &port->oldTimeouts)) {
		/* failed to set the timeouts of com port */
		goto fail_comm_state;
	}

	return 0;
fail_comm_state:
	CloseHandle(port->fd);
	port->fd = NULL;
fail_fd:
	free(port->device);
fail:
	return -1;
#else /* UNIX */
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
#endif
}

int cserial_close(struct serial_port *port)
{
#ifdef WIN32
	if (port->fd == NULL) return 0;

	PurgeComm(port->fd, PURGE_RXCLEAR | PURGE_TXCLEAR);

	if (!SetCommState(port->fd, &port->oldDCB))
		goto fail;
	if (!SetCommTimeouts(port->fd, &port->oldTimeouts))
		goto fail;
	CloseHandle(port->fd);
	port->fd = NULL;

	free(port->device);

	return 0;
fail:
	return -1;
#else
	/* restore old port settings */
	tcsetattr(port->fd, TCSANOW, &port->oldtio);
	close(port->fd);
	return 0;
#endif
}

int cserial_read(struct serial_port *port, void *buf, int size)
{
	return 0;
}

int cserial_write(struct serial_port *port, const void *buf, int size)
{
	return 0;
}

