#ifndef __HELLO_SPIBUS__
#define __HELLO_SPIBUS__

#include <stdint.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>

class SPIBus {
	public:
		SPIBus();
		spi_host_device_t getHost();
	private:
};

#endif