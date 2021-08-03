#include "spiffs.h"

#include "esp_spiffs.h"

#include "esp_log.h"
#define LOGTAG "SPIFFS"

void spiffs_init() {
	esp_vfs_spiffs_conf_t conf = {
		.base_path = "/spiffs",
		.partition_label = NULL,
		.max_files = 4,
		.format_if_mount_failed = false
	};

	ESP_LOGI(LOGTAG, "Registering");
	esp_err_t ret = esp_vfs_spiffs_register(&conf);

	if (ret != ESP_OK) {
		if (ret == ESP_FAIL) {
			ESP_LOGE(LOGTAG, "Failed to mount or format");
		} else if (ret == ESP_ERR_NOT_FOUND) {
			ESP_LOGE(LOGTAG, "Failed to find partition");
		} else {
			ESP_LOGE(LOGTAG, "Failed to initialize: %s", esp_err_to_name(ret));
		}

		return;
	} else {
		ESP_LOGI(LOGTAG, "Ready");
	}
}