#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "esp_sleep.h"
#include "esp_event.h"
#include "driver/adc.h"

#include "buttonshim.h"
#include "spibus.h"
#include "epd2in7b.h"
#include "epdpaint.h"
#include "wifi.h"
#include "httpd.h"
#include "spiffs.h"

#include "hello_config.h"

#include <esp_log.h>
#define LOGTAG "Main"

uint8_t* imgBlack;
uint8_t* imgRed;
Paint* paintBlack;
Paint* paintRed;

#include "screens/hellotag.h"
#include "screens/qrtag.h"

extern "C" {

void app_main() {
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	adc1_config_width(ADC_WIDTH_12Bit);
	adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_11db);

	int batADC = adc1_get_raw(ADC1_CHANNEL_7);
	float batV = (float)batADC * 2 * 3.3 / 4096;
	float batPct = (batV - 3.6) / (4.2 - 3.6) * 100;
	ESP_LOGW(LOGTAG, "BATTERY VOLTAGE %0.2f ~ %2.0f%%", batV, batPct);

	ESP_LOGI(LOGTAG, "ButtonShim Init");
	ButtonShim* btn = new ButtonShim(BS_I2C_PORT, BS_SCL, BS_SDA);

	ESP_LOGI(LOGTAG, "SPIBus Init");
	SPIBus* spi = new SPIBus();

	ESP_LOGI(LOGTAG, "Epd Init");
	Epd* epd = new Epd();
	epd->SpiInit(spi);
	epd->Sleep();

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

	Screen* s1 = new HelloTag(paintBlack, paintRed, "hello", "MY NAME IS", "Doogie");
	Screen* s2 = new HelloTag(paintBlack, paintRed, "saluton", "MIA NOMO ESTAS", "Mateo");
	Screen* s3 = new QrTag(paintBlack, paintRed, "Proj. Details", "https://github.com/mmdoogie/hello-badge", "GitHub: mmdoogie/hello-badge");

	uint8_t b = 0;
	b = btn->getState();
	b = btn->getState();

	if (b != 0) {
		ESP_LOGI(LOGTAG, "Button pressed at startup!");
		spiffs_init();
		if (b == ButtonShim::BUTTON_A) {
			ESP_LOGI(LOGTAG, "Starting WiFi & HTTP Server!");
			char* finalSSID = wifi_init_softap();
			start_httpd_server(imgBlack, imgRed, xTaskGetCurrentTaskHandle());

			char qrStr[64];
			char bottomStr[64];
			snprintf(qrStr, 64, "WIFI:T:WPA2;S:%s;P:%s;;", finalSSID, WIFI_SOFTAP_PSK);
			snprintf(bottomStr, 64, "%s / %s", finalSSID, WIFI_SOFTAP_PSK);
			free(finalSSID);

			Screen* sW = new QrTag(paintBlack, paintRed, "WiFi Server", qrStr, bottomStr);
			paintBlack->Clear(0);
			paintRed->Clear(0);
			sW->render();
			
			ESP_LOGI(LOGTAG, "Displaying WiFi QR");
			epd->DispInit();
			epd->DisplayFrame(imgBlack, imgRed);
			epd->Sleep();
			
			while (1) {
				ESP_LOGI(LOGTAG, "Main Task NotifyWait");
				xTaskNotifyWait(0, 0, 0, portMAX_DELAY);
				ESP_LOGI(LOGTAG, "Main Task Notified - Request Display");
				epd->DispInit();
				epd->DisplayFrame(imgBlack, imgRed);
				epd->Sleep();
			}
		} else {
			while (1) {
				if (b != 0) {
					const char* fn;
					switch (b) {
						default:
						case ButtonShim::BUTTON_B:
							fn = "/spiffs/slotB.img";
							break;
						case ButtonShim::BUTTON_C:
							fn = "/spiffs/slotC.img";
							break;
						case ButtonShim::BUTTON_D:
							fn = "/spiffs/slotD.img";
							break;
						case ButtonShim::BUTTON_E:
							fn = "/spiffs/slotE.img";
							break;
					}
					ESP_LOGI(LOGTAG, "Displaying %s", fn);
					FILE* f = fopen(fn, "rb");
					if (f == NULL) {
						ESP_LOGE(LOGTAG, "Failed to open");
						return;
					}
					int rx = fread(imgBlack, 1, 5808, f);
					if (rx != 5808) {
						ESP_LOGE(LOGTAG, "File short black %d", rx);
					}
					rx = fread(imgRed, 1, 5808, f);
					if (rx != 5808) {
						ESP_LOGE(LOGTAG, "File short red %d", rx);
					}
					fclose(f);
					ESP_LOGI(LOGTAG, "File Read - Request Display");
					epd->DispInit();
					epd->DisplayFrame(imgBlack, imgRed);
					epd->Sleep();
				}

				esp_sleep_enable_timer_wakeup(500000);
				esp_light_sleep_start();
				ESP_LOGI(LOGTAG, "Wakeup: %d", esp_sleep_get_wakeup_cause());
				b = btn->getState();
				ESP_LOGI("Buttons", "%02X", b);
			}
		}
	} else {
		ESP_LOGI(LOGTAG, "No button pressed at startup, normal mode!");
		while (1) {
			ESP_LOGI(LOGTAG, "Reading Buttons");
			uint8_t b = 0;
			while (b == 0) {
				esp_sleep_enable_timer_wakeup(500000);
				esp_light_sleep_start();
				ESP_LOGI(LOGTAG, "Wakeup: %d", esp_sleep_get_wakeup_cause());
				b = btn->getState();
				ESP_LOGI("Buttons", "%02X", b);
			}

			ESP_LOGI(LOGTAG, "Clearing Paints");
			paintBlack->Clear(0);
			paintRed->Clear(0);

			switch (b) {
				case ButtonShim::BUTTON_A:
					s1->render();
					break;
				case ButtonShim::BUTTON_B:
					s2->render();
					break;
				case ButtonShim::BUTTON_C:
					s3->render();
					break;
			}

			ESP_LOGI(LOGTAG, "Request Display");
			epd->DispInit();
			epd->DisplayFrame(imgBlack, imgRed);
			epd->Sleep();
		}
	}
}

}
