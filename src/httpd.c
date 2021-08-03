#include <stdio.h>
#include "httpd.h"

#include "esp_http_server.h"

#include "esp_log.h"
#define LOGTAG "HTTPD"

#define MIN(a,b) ((a) < (b) ? (a) : (b))

extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

static uint8_t* _imgRed = 0;
static uint8_t* _imgBlack = 0;
static const int _imgSize = 5808;
static TaskHandle_t _mainTaskHandle = 0;

static esp_err_t http_get_handler(httpd_req_t *req) {
    httpd_resp_set_status(req, HTTPD_200);
    httpd_resp_send(req, (const char*)index_html_start, index_html_end-index_html_start);
    return ESP_OK;
}

static esp_err_t http_post_handler(httpd_req_t *req) {
	char dest = req->uri[8];
	
	if (dest < 'A' || dest > 'E') {
		httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid Slot Destination");
        return ESP_FAIL;
	}

	if (req->content_len != _imgSize*2) {
		httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid Content Length");
        return ESP_FAIL;
	}

    ESP_LOGI(LOGTAG, "Receiving Slot %c", dest);

    int received = 0;
    int remaining = 5808;
	int retryCount = 0;

    while (remaining > 0) {
        ESP_LOGI(LOGTAG, "Black remaining size : %d", remaining);
        if ((received = httpd_req_recv(req, (char*)_imgBlack+_imgSize-remaining, MIN(remaining, _imgSize))) <= 0) {
            if (received == HTTPD_SOCK_ERR_TIMEOUT && retryCount < 4) {
                retryCount++;
                continue;
            }

            ESP_LOGE(LOGTAG, "File reception failed!");
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to receive file");
            return ESP_FAIL;
        }
        remaining -= received;
    }

	received = 0;
	remaining = _imgSize;
	retryCount = 0;

    while (remaining > 0) {
        ESP_LOGI(LOGTAG, "Red remaining size : %d", remaining);
        if ((received = httpd_req_recv(req, (char*)_imgRed+_imgSize-remaining, MIN(remaining, _imgSize))) <= 0) {
            if (received == HTTPD_SOCK_ERR_TIMEOUT && retryCount < 4) {
                retryCount++;
                continue;
            }

            ESP_LOGE(LOGTAG, "File reception failed!");
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to receive file");
            return ESP_FAIL;
        }
        remaining -= received;
    }

    ESP_LOGI(LOGTAG, "Receive complete!");

    httpd_resp_set_status(req, HTTPD_200);
    httpd_resp_send(req, 0, 0);

	if (dest != 'A') {
		char fn[18] = "/spiffs/slotX.img";
		fn[12] = dest;
		ESP_LOGI(LOGTAG, "Writing to %s", fn);
		FILE* f = fopen(fn, "wb");
		if (f == NULL) {
			ESP_LOGE(LOGTAG, "Error opening for write.");
			return ESP_FAIL;
		}
		int tx = fwrite(_imgBlack, 1, _imgSize, f);
		if (tx != _imgSize) {
			ESP_LOGE(LOGTAG, "Short write black %d", tx);
			return ESP_FAIL;
		}
		tx = fwrite(_imgRed, 1, _imgSize, f);
		if (tx != _imgSize) {
			ESP_LOGE(LOGTAG, "Short write red %d", tx);
			return ESP_FAIL;
		}
		fclose(f);
		ESP_LOGI(LOGTAG, "Successfully written.");
	}

	xTaskNotify(_mainTaskHandle, 0, eNoAction);

    return ESP_OK;
}

esp_err_t start_httpd_server(uint8_t* imgBlack, uint8_t* imgRed, TaskHandle_t mainTaskHandle) {
	_imgBlack = imgBlack;
	_imgRed = imgRed;
	_mainTaskHandle = mainTaskHandle;

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(LOGTAG, "Starting");
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(LOGTAG, "Failed to start file server!");
        return ESP_FAIL;
    }

    httpd_uri_t get_uri_handler = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = http_get_handler,
		.user_ctx  = 0
    };
    httpd_register_uri_handler(server, &get_uri_handler);

    httpd_uri_t post_uri_handler = {
        .uri       = "/submit/*",
        .method    = HTTP_POST,
        .handler   = http_post_handler,
		.user_ctx  = 0
    };
    httpd_register_uri_handler(server, &post_uri_handler);

    return ESP_OK;
}