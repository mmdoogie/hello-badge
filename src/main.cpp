#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "buttonshim.h"

#include "hello_config.h"

extern "C" {

void app_main() {
	ButtonShim* btn = new ButtonShim(BS_I2C_PORT, BS_SCL, BS_SDA);
	
	while (1) {
		ESP_LOGE("BUTTONS", "%02X", btn->getState());
		vTaskDelay(500 / portTICK_RATE_MS);
	}
}

}