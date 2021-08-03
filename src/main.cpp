#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "esp_sleep.h"

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

#include "screens/hellotag.h"
#include "screens/qrtag.h"

#include "esp_wifi.h"
#include "esp_interface.h"
#include "esp_event.h"
#include "esp_system.h"
#include "esp_http_server.h"

TaskHandle_t mth;

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(LOGTAG, "station %02x:%02x:%02x:%02x:%02x:%02x join, AID=%d", MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(LOGTAG, "station %02x:%02x:%02x:%02x:%02x:%02x leave, AID=%d", MAC2STR(event->mac), event->aid);
    }
}

void wifi_init_softap()
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	cfg.nvs_enable = false;
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));

    wifi_config_t wifi_config = {
        .ap = {
            "HelloBadge",
			"t0mat03s",
            10,
            1,
            WIFI_AUTH_WPA_WPA2_PSK,
			0,
            1,
			100
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(LOGTAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d", "HelloBadge", "t0mat03s", 1);
}

extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

static esp_err_t download_get_handler(httpd_req_t *req) {
    httpd_resp_set_status(req, HTTPD_200);
    httpd_resp_send(req, (const char*)index_html_start, index_html_end-index_html_start);
    return ESP_OK;
}

#define MIN(a,b) ((a) < (b) ? (a) : (b))

static esp_err_t upload_post_handler(httpd_req_t *req) {
	char dest = req->uri[8];
	
	if (dest < 'A' || dest > 'E') {
		httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid Slot Destination");
        return ESP_FAIL;
	}

	if (req->content_len != 11616) {
		httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Content Length Must Be 11616");
        return ESP_FAIL;
	}

    ESP_LOGI(LOGTAG, "Receiving file slot %c...", dest);

    int received = 0;
    int remaining = 5808;

    while (remaining > 0) {
        ESP_LOGI(LOGTAG, "Remaining size : %d", remaining);
        if ((received = httpd_req_recv(req, (char*)imgBlack+5808-remaining, MIN(remaining, 5808))) <= 0) {
            if (received == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry if timeout occurred */
                continue;
            }

            ESP_LOGE(LOGTAG, "File reception failed!");
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to receive file");
            return ESP_FAIL;
        }
        remaining -= received;
    }

	received = 0;
	remaining = 5808;

    while (remaining > 0) {
        ESP_LOGI(LOGTAG, "Remaining size : %d", remaining);
        if ((received = httpd_req_recv(req, (char*)imgRed+5808-remaining, MIN(remaining, 5808))) <= 0) {
            if (received == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry if timeout occurred */
                continue;
            }

            ESP_LOGE(LOGTAG, "File reception failed!");
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to receive file");
            return ESP_FAIL;
        }
        remaining -= received;
    }

    ESP_LOGI(LOGTAG, "File reception complete");

    httpd_resp_set_status(req, HTTPD_200);
    httpd_resp_send(req, 0, 0);

	xTaskNotify(mth, 0, eNoAction);

    return ESP_OK;
}

esp_err_t start_file_server()
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    /* Use the URI wildcard matching function in order to
     * allow the same handler to respond to multiple different
     * target URIs which match the wildcard scheme */
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(LOGTAG, "Starting HTTP Server");
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(LOGTAG, "Failed to start file server!");
        return ESP_FAIL;
    }

    httpd_uri_t file_download = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = download_get_handler,
		.user_ctx  = 0
    };
    httpd_register_uri_handler(server, &file_download);

    httpd_uri_t file_upload = {
        .uri       = "/submit/*",   // Match all URIs of type /upload/path/to/file
        .method    = HTTP_POST,
        .handler   = upload_post_handler,
		.user_ctx  = 0
    };
    httpd_register_uri_handler(server, &file_upload);

    return ESP_OK;
}

extern "C" {

void app_main() {
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
		if (b == ButtonShim::BUTTON_A) {
			ESP_LOGI(LOGTAG, "Starting WiFi!");
			wifi_init_softap();
			start_file_server();
			mth = xTaskGetCurrentTaskHandle();
			while (1) {
				ESP_LOGI(LOGTAG, "Main Task NotifyWait");
				xTaskNotifyWait(0, 0, 0, portMAX_DELAY);
				ESP_LOGI(LOGTAG, "Main Task Notified - Request Display");
				epd->DispInit();
				epd->DisplayFrame(imgBlack, imgRed);
				epd->Sleep();
			}
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
