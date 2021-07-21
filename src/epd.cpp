#include "epd.h"
#include "hello_config.h"

EPD::EPD(SPIBus* bus) {
	_spi_host = bus->getHost();

	spi_device_interface_config_t c;
	c.address_bits = 8;
	c.command_bits = 8;
	c.mode = 0;
	c.clock_speed_hz = 4000000;
	c.spics_io_num = SPI_CS_EPD;
	c.flags = 0;

	spi_bus_add_device(_spi_host, &c, &_spi_device_handle);
}