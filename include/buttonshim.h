#include <stdint.h>
#include <driver/i2c.h>
#include "tca9554a.h"

class ButtonShim {
	public:
		ButtonShim(i2c_port_t i2c_port, gpio_num_t i2c_scl_gpio, gpio_num_t i2c_sda_gpio);
		uint8_t getState();
		typedef enum : uint8_t {
			BUTTON_A = 1,
			BUTTON_B = 2,
			BUTTON_C = 4,
			BUTTON_D = 8,
			BUTTON_E = 16
		} buttons;
	private:
		TCA9554A* _iox;
};