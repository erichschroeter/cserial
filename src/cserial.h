#ifndef __CSERIAL_H
#define __CSERIAL_H

#ifdef _WIN32
  #ifdef cserial_EXPORTS
    #define CSERIALAPI __declspec(dllexport)
  #else
    #define CSERIALAPI __declspec(dllimport)
  #endif
  #define CSERIALCALL __cdecl
#else
  #define CSERIALAPI
  #define CSERIALCALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32

#include <windows.h>

#else /* UNIX */

#include <termios.h>
#include <signal.h>

#endif

CSERIALAPI const char * CSERIALCALL cserial_strerror(int errnum);

struct cserial_port {
#ifdef WIN32
	HANDLE fd;
	DCB dcb;
	DCB oldDCB;
	COMMTIMEOUTS timeouts;
	COMMTIMEOUTS oldTimeouts;
#else /* UNIX */
	int fd;
	struct termios oldtio;
	struct termios tio;
#endif
	char *device;
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

CSERIALAPI int CSERIALCALL cserial_open(struct cserial_port *port,
	struct cserial_port_conf *conf, const char *device);
CSERIALAPI int CSERIALCALL cserial_init(struct cserial_port *port, struct cserial_port_conf *conf);
CSERIALAPI int CSERIALCALL cserial_close(struct cserial_port *port);
CSERIALAPI void CSERIALCALL cserial_free(struct cserial_port *port);
CSERIALAPI int CSERIALCALL cserial_read(struct cserial_port *port, void *buf, int size);
CSERIALAPI int CSERIALCALL cserial_write(struct cserial_port *port, const void *buf, int size);

#ifdef __cplusplus
}
#endif

#endif /* __CSERIAL_H */
