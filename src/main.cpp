#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "buttonshim.h"
#include "spibus.h"
#include "epd.h"

#include "hello_config.h"

extern "C" {

void app_main() {
	ButtonShim* btn = new ButtonShim(BS_I2C_PORT, BS_SCL, BS_SDA);
	
	SPIBus* spi = new SPIBus();
	EPD* epd = new EPD(spi);

	while (1) {
		ESP_LOGE("BUTTONS", "%02X", btn->getState());
		vTaskDelay(500 / portTICK_RATE_MS);
	}
}

}