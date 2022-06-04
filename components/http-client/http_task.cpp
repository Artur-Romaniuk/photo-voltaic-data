#include "HttpClient.hpp"
#include "esp_log.h"
#include "wifi_init.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "Weather API"

void http_task(void * /*pvParameters*/) {
    while (true) {
        /**
         * @brief Waiting for WiFi connection before starting http service.
         *
         */
        EventBits_t sc_bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, true, false, portMAX_DELAY);
        if (sc_bits & WIFI_CONNECTED_BIT) {
            break;
        }
    }

    HttpClient http;
    TickType_t last_wake_time  = xTaskGetTickCount();
    const TickType_t frequency = 60000 / portTICK_PERIOD_MS; /**< Fetch API every 1min. */

    while (true) {
        vTaskDelayUntil(&last_wake_time, frequency);

        while (http.perform() != ESP_OK) {
            vTaskDelay(100 / portTICK_PERIOD_MS); /**< HTTPS fails pretty often. No matter the reason try reconnecting after 100ms. */
        }
    }
    vTaskDelete(nullptr);
}