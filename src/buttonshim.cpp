#include "buttonshim.h"

ButtonShim::ButtonShim(i2c_port_t i2c_port, gpio_num_t i2c_scl_gpio, gpio_num_t i2c_sda_gpio) {
	_iox = new TCA9554A(i2c_port, 0x3F, i2c_scl_gpio, i2c_sda_gpio, 100000);
	_iox->setConfig(0x1F);
	_iox->setOutput(0x00);
	_iox->setPolarity(0x1F);
}

uint8_t ButtonShim::getState() {
	return _iox->getInput() & 0x1F;
}
