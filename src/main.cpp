#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "buttonshim.h"
#include "spibus.h"
#include "epd2in7b.h"
#include "epdpaint.h"

#include "hello_config.h"

#include <esp_log.h>
#define LOGTAG "Main"

uint8_t* imgBlack;
uint8_t* imgRed;
Paint* paintBlack;
Paint* paintRed;
#define COLOR 1
#define BLANK 0

extern "C" {

void app_main() {
	ESP_LOGI(LOGTAG, "ButtonShim Init");
	ButtonShim* btn = new ButtonShim(BS_I2C_PORT, BS_SCL, BS_SDA);

	ESP_LOGI(LOGTAG, "SPIBus Init");
	SPIBus* spi = new SPIBus();

	ESP_LOGI(LOGTAG, "Epd Init");
	Epd* epd = new Epd();
	epd->SpiInit(spi);
	epd->DispInit();

	ESP_LOGI(LOGTAG, "Alloc Buffs");
	imgBlack = (uint8_t*)malloc(EPD_WIDTH * EPD_HEIGHT / 8);
	imgRed = (uint8_t*)malloc(EPD_WIDTH * EPD_HEIGHT / 8);

	ESP_LOGI(LOGTAG, "Paint Init");
	paintBlack = new Paint(imgBlack, EPD_HEIGHT, EPD_WIDTH);
	paintBlack->SetRotate(Paint::ROTATE_270);
	paintBlack->SetInvert(false);

	paintRed = new Paint(imgRed, EPD_HEIGHT, EPD_WIDTH);
	paintRed->SetRotate(Paint::ROTATE_270);
	paintRed->SetInvert(true);

	ESP_LOGI(LOGTAG, "Reading Buttons");
	uint8_t b = 0;
	while (b != 1) {
		b = btn->getState();
		ESP_LOGE("Buttons", "%02X", b);
		vTaskDelay(500 / portTICK_RATE_MS);
	}

	ESP_LOGI(LOGTAG, "Button Pressed");
	ESP_LOGI(LOGTAG, "Clearing Paints");
	paintBlack->Clear(BLANK);
	paintRed->Clear(BLANK);

	ESP_LOGI(LOGTAG, "Drawing");
	paintBlack->DrawFilledRectangle(10, 10, 50, 50, COLOR);
	paintRed->DrawFilledCircle(50, 50, 20, COLOR);
	paintRed->DrawFilledRectangle(10, 10, 50, 50, BLANK);
	
	ESP_LOGI(LOGTAG, "Start Refresh");
	epd->DisplayFrame(imgBlack, imgRed);
}

}