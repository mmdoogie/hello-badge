#ifndef __HELLO_TCA9554A__
#define __HELLO_TCA9554A__

#include <stdint.h>
#include <driver/i2c.h>

class TCA9554A {
	public:
		TCA9554A(i2c_port_t i2c_port, uint8_t i2c_addr, gpio_num_t i2c_scl_gpio, gpio_num_t i2c_sda_gpio, uint32_t i2c_freq);
		void setConfig(uint8_t val);
		void setPolarity(uint8_t val);
		void setOutput(uint8_t val);
		uint8_t getInput();
	private:
		i2c_port_t _i2c_port;
		uint8_t _i2c_addr;
		void _write_reg(uint8_t reg, uint8_t val);
		uint8_t _read_reg(uint8_t reg);
		typedef enum : uint8_t {
			INPUT = 0,
			OUTPUT = 1,
			POLARITY = 2,
			CONFIG = 3
		} _reg_addr;
};

#endif