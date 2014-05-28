#include "cserial.h"

#ifdef WIN32
#else /* UNIX */

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#endif

CSERIALAPI const char * CSERIALCALL cserial_strerror(int errnum)
{
	switch (errnum) {
	default:
		return strerror(errnum);
	}
}

int cserial_init(struct cserial_port *port, struct cserial_port_conf *conf)
{
	unsigned int baud, csize, stopbits, parity, flowcontrol_hw;
	int ret = 0;
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
	tcgetattr(port->fd, &port->tio);
	/*cfmakeraw(&port->tio);*/

	/* Baud rate */
	if (conf->baud >= 230400) {
		baud = B230400;
	} else if (conf->baud >= 115200) {
		baud = B115200;
	} else if (conf->baud >= 57600) {
		baud = B57600;
	} else if (conf->baud >= 38400) {
		baud = B38400;
	} else if (conf->baud >= 19200) {
		baud = B19200;
	} else if (conf->baud >= 9600) {
		baud = B9600;
	} else if (conf->baud >= 4800) {
		baud = B4800;
	} else if (conf->baud >= 2400) {
		baud = B2400;
	} else if (conf->baud >= 1800) {
		baud = B1800;
	} else if (conf->baud >= 1200) {
		baud = B1200;
	} else if (conf->baud >= 600) {
		baud = B600;
	} else if (conf->baud >= 300) {
		baud = B300;
	} else if (conf->baud >= 200) {
		baud = B200;
	} else if (conf->baud >= 150) {
		baud = B150;
	} else if (conf->baud >= 134) {
		baud = B134;
	} else if (conf->baud >= 110) {
		baud = B110;
	} else if (conf->baud >= 75) {
		baud = B75;
	} else if (conf->baud >= 50) {
		baud = B50;
	} else {
		baud = B0;
	}
	cfsetospeed(&port->tio, baud);
	cfsetispeed(&port->tio, baud);

	/* Character size */
	if (conf->csize >= 8) {
		csize = CS8;
	} else if (conf->csize >= 7) {
		csize = CS7;
	} else if (conf->csize >= 6) {
		csize = CS6;
	} else {
		csize = CS5;
	}

	/* Parity */
	switch (conf->parity) {
	case PARITY_ODD:
		parity = (PARENB | PARODD);
		break;
	case PARITY_EVEN:
		parity = (PARENB);
		parity &= ~(PARODD);
		break;
	case PARITY_NONE:
	default:
		parity = 0;
		break;
	}

	/* Stop bits */
	if (conf->stopbits >= 2) {
		stopbits = CSTOPB;
	} else {
		stopbits = 0;
	}

	/* Hardware Flow Control */
	if (conf->flowcontrol_hw) {
		flowcontrol_hw = CRTSCTS; /* Enable RTS/CTS (hardware) flow control */
	} else {
		flowcontrol_hw = 0;
	}

	port->tio.c_cflag |= (
		csize |
		parity |
		stopbits |
		flowcontrol_hw |
		CREAD |          /* Enable receiver */
		CLOCAL |         /* Ignore modem control lines */
		HUPCL);
	port->tio.c_iflag |= (IGNBRK);
	port->tio.c_cc[VMIN] = 1;
	port->tio.c_cc[VTIME] = 5;

	/* clean the modem line and activate the settings for the port */
	tcflush(port->fd, TCIFLUSH);
	if (tcsetattr(port->fd, TCSANOW, &port->tio) < 0) { ret = errno; goto fail; }

fail:
	return ret;
#endif
}

int cserial_open(struct cserial_port *port, struct cserial_port_conf *conf, char *device)
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

	if (conf) {
		ret = cserial_init(port, conf);
		if (ret) { goto fail_tty; }
	}

	return ret;
fail_tty:
	close(port->fd);
fail:
	return ret;
#endif
}

void cserial_free(struct cserial_port *port)
{
	if (port == NULL)
		return;

	if (port->device) {
		free(port->device);
	}

	free(port);
}

int cserial_close(struct cserial_port *port)
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
#else /* UNIX */
	/* restore old port settings */
	tcsetattr(port->fd, TCSANOW, &port->oldtio);
	close(port->fd);
	port->fd = NULL;

	return 0;
#endif
}

int cserial_read(struct cserial_port *port, void *buf, int size)
{
	int ret;
#ifdef WIN32
#else /* UNIX */
	ret = read(port->fd, buf, size);
#endif
	return ret;
}

int cserial_write(struct cserial_port *port, const void *buf, int size)
{
	int ret;
#ifdef WIN32
#else /* UNIX */
	ret = write(port->fd, buf, size);
#endif
	return ret;
}

