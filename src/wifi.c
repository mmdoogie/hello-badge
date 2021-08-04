#include "wifi.h"
#include "hello_config.h"

#include <string.h>
#include "esp_wifi.h"

#include "esp_log.h"
#define LOGTAG "WiFi"

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(LOGTAG, "station %02x%02x%02x%02x%02x%02x join, AID=%d", MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(LOGTAG, "station %02x%02x%02x%02x%02x%02x leave, AID=%d", MAC2STR(event->mac), event->aid);
    }
}

void wifi_init_softap() {
    ESP_LOGI(LOGTAG, "Starting TCPIP");
    tcpip_adapter_init();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	cfg.nvs_enable = false;
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));

	uint8_t baseMac[6];
	esp_efuse_mac_get_default(&baseMac[0]);
	
    wifi_config_t wifi_config = {
        .ap = {
			.password = WIFI_SOFTAP_PSK,
            .channel = 1,
            .authmode = WIFI_AUTH_WPA2_PSK,
			.ssid_hidden = 0,
            .max_connection = 4,
			.beacon_interval = 100
        }
    };
	snprintf((char*)wifi_config.ap.ssid, 32, "%s%02x%02x", WIFI_SOFTAP_SSID, baseMac[4], baseMac[5]);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(LOGTAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d", wifi_config.ap.ssid, WIFI_SOFTAP_PSK, 1);
}