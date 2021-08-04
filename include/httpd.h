#ifndef __HELLO_HTTPD__
#define __HELLO_HTTPD__

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t start_httpd_server(uint8_t* imgBlack, uint8_t* imgRed, TaskHandle_t mainTaskHandle);

#ifdef __cplusplus
}
#endif

#endif