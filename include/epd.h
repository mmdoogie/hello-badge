#include <stdint.h>
#include "spibus.h"

class EPD {
	public:
		EPD(SPIBus* bus);
	private:
		spi_host_device_t _spi_host;
		spi_device_handle_t _spi_device_handle;
};
