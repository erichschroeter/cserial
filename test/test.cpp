#define CATCH_CONFIG_MAIN

#include <stdlib.h>
#include <string.h>

#include "catch.hpp"

#include <cserial.h>

#ifdef _WIN32
#define snprintf _snprintf_s
#define sprintf sprintf_s
#define strcat strcat_s
#endif

TEST_CASE("Loopback send/receive", "[tx][rx][loopback]")
{
	struct cserial_port txPort, rxPort;
	struct cserial_port_conf conf = {
		conf.baud = 19200,
		conf.parity = PARITY_NONE,
		conf.csize = 8,
		conf.stopbits = 1,
	};
	char *txDev, *rxDev;
	char tx[32], rx[32];
	int txLen;
	int ret;

	memset(tx, 0, sizeof(tx));
	memset(rx, 0, sizeof(rx));

	snprintf(tx, sizeof(tx), "Erich");

	/* Get the device file to transmit data out. */
	txDev = getenv("CSERIAL_TX");
	INFO("transmitting using: " << txDev);
	REQUIRE(txDev != 0);

	/* Get the device file to receive data in. */
	rxDev = getenv("CSERIAL_RX");
	INFO("receiving using: " << rxDev);
	REQUIRE(rxDev != 0);

	ret = cserial_open(&txPort, &conf, txDev);
	INFO("tx cserial_open: " << cserial_strerror(ret));
	REQUIRE(ret == 0);
	INFO("rx cserial_open: " << cserial_strerror(ret));
	ret = cserial_open(&rxPort, &conf, rxDev);
	REQUIRE(ret == 0);

	txLen = strlen(tx);
	ret = cserial_write(&txPort, tx, txLen);
	REQUIRE(ret == txLen);

	ret = cserial_read(&rxPort, rx, sizeof(rx));
	/* We should receive everything we transmitted. */
	REQUIRE(ret == txLen);

	ret = cserial_close(&txPort);
	ret = cserial_close(&rxPort);
	REQUIRE(ret == 0);

	free(txPort.device);
	free(rxPort.device);
}

