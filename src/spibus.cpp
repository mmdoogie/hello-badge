#include "spibus.h"
#include "hello_config.h"

SPIBus::SPIBus() {
	spi_bus_config_t c;
	c.miso_io_num = SPI_MISO;
	c.mosi_io_num = SPI_MOSI;
	c.sclk_io_num = SPI_SCK;
	c.quadhd_io_num = -1;
	c.quadwp_io_num = -1;
	c.max_transfer_sz = 0;
	c.intr_flags = 0;
	c.flags = SPICOMMON_BUSFLAG_MASTER;

	spi_bus_initialize(SPI3_HOST, &c, 0);
}

spi_host_device_t SPIBus::getHost() {
	return SPI3_HOST;
}