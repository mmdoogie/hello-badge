#include "tca9554a.h"

TCA9554A::TCA9554A(i2c_port_t i2c_port, uint8_t i2c_addr, gpio_num_t i2c_scl_gpio, gpio_num_t i2c_sda_gpio, uint32_t i2c_freq) {
	_i2c_port = i2c_port;
	_i2c_addr = i2c_addr;

	i2c_config_t c;
	c.mode = I2C_MODE_MASTER;
	c.scl_io_num = i2c_scl_gpio;
	c.sda_io_num = i2c_sda_gpio;
	c.scl_pullup_en = GPIO_PULLUP_ENABLE;
	c.sda_pullup_en = GPIO_PULLUP_ENABLE;
	c.master.clk_speed = i2c_freq;

	i2c_param_config(_i2c_port, &c);
	i2c_driver_install(_i2c_port, I2C_MODE_MASTER, 0, 0, 0);
}

void TCA9554A::setConfig(uint8_t val) {
	_write_reg(_reg_addr::CONFIG, val);
}

void TCA9554A::setPolarity(uint8_t val) {
	_write_reg(_reg_addr::POLARITY, val);
}

void TCA9554A::setOutput(uint8_t val) {
	_write_reg(_reg_addr::OUTPUT, val);
}

uint8_t TCA9554A::getInput() {
	return _read_reg(_reg_addr::INPUT);
}

void TCA9554A::_write_reg(uint8_t reg, uint8_t val) {
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, _i2c_addr << 1, true);
	i2c_master_write_byte(cmd, reg & 0x03, true);
	i2c_master_write_byte(cmd, val, false);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(_i2c_port, cmd, 100 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
}

uint8_t TCA9554A::_read_reg(uint8_t reg) {
	uint8_t val;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, _i2c_addr << 1, true);
	i2c_master_write_byte(cmd, _reg_addr::INPUT, false);	
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (_i2c_addr << 1) | 1, true);
	i2c_master_read_byte(cmd, &val, I2C_MASTER_NACK);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(_i2c_port, cmd, 100 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	return val;
}