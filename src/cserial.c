#include "cserial.h"

#ifdef WIN32
#else /* UNIX */

#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#endif

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

CSERIALAPI const char * CSERIALCALL cserial_strerror(int errnum)
{
	switch (errnum) {
	default:
		return strerror(errnum);
	}
}

CSERIALAPI int CSERIALCALL cserial_init(struct cserial_port *port,
	struct cserial_port_conf *conf)
{
	int ret = 0;
#ifdef WIN32
	/* Baud rate */
	if (conf->baud >= 256000) {
		port->dcb.BaudRate = CBR_256000;
	} else if (conf->baud >= 128000) {
		port->dcb.BaudRate = CBR_128000;
	} else if (conf->baud >= 115200) {
		port->dcb.BaudRate = CBR_115200;
	} else if (conf->baud >= 57600) {
		port->dcb.BaudRate = CBR_57600;
	} else if (conf->baud >= 38400) {
		port->dcb.BaudRate = CBR_38400;
	} else if (conf->baud >= 19200) {
		port->dcb.BaudRate = CBR_19200;
	} else if (conf->baud >= 9600) {
		port->dcb.BaudRate = CBR_9600;
	} else if (conf->baud >= 4800) {
		port->dcb.BaudRate = CBR_4800;
	} else if (conf->baud >= 2400) {
		port->dcb.BaudRate = CBR_2400;
	} else if (conf->baud >= 1200) {
		port->dcb.BaudRate = CBR_1200;
	} else if (conf->baud >= 600) {
		port->dcb.BaudRate = CBR_600;
	} else if (conf->baud >= 300) {
		port->dcb.BaudRate = CBR_300;
	} else if (conf->baud >= 110) {
		port->dcb.BaudRate = CBR_110;
	} else {
		port->dcb.BaudRate = CBR_115200;
	}

	/* Character size */
	if (conf->csize <= 0) {
		/* Default to 8 bit char. */
		port->dcb.ByteSize = 8;
	} else {
		port->dcb.ByteSize = conf->csize;
	}

	/* Parity */
	/* Unsupported: [ MARKPARITY, SPACEPARITY ] */
	port->dcb.fParity = TRUE;
	switch (conf->parity) {
	case PARITY_ODD:
		port->dcb.Parity = ODDPARITY;
		break;
	case PARITY_EVEN:
		port->dcb.Parity = EVENPARITY;
		break;
	case PARITY_NONE:
	default:
		port->dcb.Parity = NOPARITY;
		port->dcb.fParity = FALSE; /* Disable parity check */
		break;
	}

	/* Stop bits */
	/* Unsupported: ONE5STOPBITS */
	if (conf->stopbits >= 2) {
		port->dcb.StopBits = TWOSTOPBITS;
	} else {
		port->dcb.StopBits = ONESTOPBIT;
	}

	if (!SetCommState(port->fd, &port->dcb)) {
		/* failed to set the state of com port */
		ret = GetLastError();
		goto fail;
	}

	port->timeouts.ReadIntervalTimeout = 50;
	port->timeouts.ReadTotalTimeoutConstant = 50;
	port->timeouts.ReadTotalTimeoutMultiplier = 10;
	port->timeouts.WriteTotalTimeoutConstant = 50;
	port->timeouts.WriteTotalTimeoutMultiplier = 10;

	if (!SetCommTimeouts(port->fd, &port->timeouts)) {
		/* failed to set the timeouts of com port */
		ret = GetLastError();
		goto fail;
	}

#else /* UNIX */
	unsigned int baud, csize, stopbits, parity, flowcontrol_hw;

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

#endif
fail:
	return ret;
}

CSERIALAPI int CSERIALCALL cserial_open(struct cserial_port *port,
	struct cserial_port_conf *conf, const char *device)
{
	int ret = 0;
#ifdef WIN32
	LPCSTR _device = (LPCSTR) device;

	/* Keep reference to device opened. */
	port->device = calloc(strlen(_device) + 1, sizeof(char));
	if (!port->device) {
		ret = ENOMEM;
		goto fail;
	}
	strncpy(port->device, _device, strlen(_device));

	port->fd = CreateFile(_device, GENERIC_READ | GENERIC_WRITE,
		0, 0, OPEN_EXISTING, 0, 0);

	if (port->fd == INVALID_HANDLE_VALUE) {
		port->fd = NULL;
		ret = GetLastError();
		goto fail_fd;
	}

	if (!GetCommState(port->fd, &port->oldDCB)) {
		/* could not get the state of com port */
		ret = GetLastError();
		goto fail_comm_state;
	}

	if (!GetCommTimeouts(port->fd, &port->oldTimeouts)) {
		/* failed to set the timeouts of com port */
		ret = GetLastError();
		goto fail_comm_state;
	}

	goto success;
fail_comm_state:
	CloseHandle(port->fd);
	port->fd = NULL;
fail_fd:
	free(port->device);
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

	goto success;
fail_tty:
	close(port->fd);
#endif
fail:
	return ret;

success:
	/* Auto-configure if conf was specified. */
	if (conf) {
		ret = cserial_init(port, conf);
	}

	return ret;
}

CSERIALAPI void CSERIALCALL cserial_free(struct cserial_port *port)
{
	if (port == NULL)
		return;

	if (port->device) {
		free(port->device);
	}

	free(port);
}

CSERIALAPI int CSERIALCALL cserial_close(struct cserial_port *port)
{
	int ret = 0;

	if (port->fd == NULL) goto success;
#ifdef WIN32

	PurgeComm(port->fd, PURGE_RXCLEAR | PURGE_TXCLEAR);

	if (!SetCommState(port->fd, &port->oldDCB)) {
		ret = GetLastError();
		goto fail;
	}
	if (!SetCommTimeouts(port->fd, &port->oldTimeouts)) {
		ret = GetLastError();
		goto fail;
	}
	CloseHandle(port->fd);

	goto success;
#else /* UNIX */
	/* restore old port settings */
	if (tcsetattr(port->fd, TCSANOW, &port->oldtio) < 0) {
		ret = errno;
		goto fail;
	}
	close(port->fd);

	goto success;
#endif
fail:
	return ret;
success:
	free(port->device);
	port->fd = NULL;
	return 0;
}

CSERIALAPI int CSERIALCALL cserial_read(struct cserial_port *port,
	void *buf, int size)
{
	int ret;
#ifdef WIN32
	DWORD bytesRead;
	ret = ReadFile(port->fd, buf, size, &bytesRead, NULL);
#else /* UNIX */
	ret = read(port->fd, buf, size);
#endif
	return ret;
}

CSERIALAPI int CSERIALCALL cserial_write(struct cserial_port *port,
	const void *buf, int size)
{
	int ret;
#ifdef WIN32
	DWORD bytesWritten;
	ret = WriteFile(port->fd, buf, size, &bytesWritten, NULL);
#else /* UNIX */
	ret = write(port->fd, buf, size);
#endif
	return ret;
}

