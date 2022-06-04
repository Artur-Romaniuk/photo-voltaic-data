#include "HttpClient.hpp"
#include "esp_log.h"
#include "wifi_init.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "Weather API"

void http_task(void * /*pvParameters*/) {
    while (true) {
        EventBits_t sc_bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, true, false, portMAX_DELAY);
        if (sc_bits & WIFI_CONNECTED_BIT) {
            break;
        }
    }

    HttpClient http;
    TickType_t last_wake_time  = xTaskGetTickCount();
    const TickType_t frequency = 60000 / portTICK_PERIOD_MS;

    while (true) {
        while (http.perform() != ESP_OK) {
            vTaskDelay(100);
        }
        vTaskDelayUntil(&last_wake_time, frequency);
    }
    vTaskDelete(nullptr);
}