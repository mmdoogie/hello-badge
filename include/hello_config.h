#ifndef __HELLO_CONFIG__
#define __HELLO_CONFIG__

// Pimoroni Button SHIM wiring details
#define BS_I2C_PORT		I2C_NUM_0
#define BS_SCL			GPIO_NUM_22
#define BS_SDA			GPIO_NUM_23

// Common SPI bus wiring details
#define SPI_SCK			GPIO_NUM_5
#define SPI_MOSI		GPIO_NUM_18
#define SPI_MISO		GPIO_NUM_19

// EPD-specific SPI wiring details
#define SPI_CS_EPD		GPIO_NUM_21
#define EPD_DC			GPIO_NUM_14
#define EPD_RST			GPIO_NUM_33
#define EPD_BUSY		GPIO_NUM_27
#define EPD_ENA			GPIO_NUM_13

// Chip Selects for SRAM and SD
#define SPI_CS_SRAM		GPIO_NUM_32
#define SPI_CS_SD		GPIO_NUM_15

#endif