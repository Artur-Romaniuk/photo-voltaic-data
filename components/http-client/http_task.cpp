#include "HttpClient.hpp"
#include "esp_log.h"
#include "wifi_init.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "Weather API"

void http_task(void * /*pvParameters*/) {
    HttpClient http;
    TickType_t last_wake_time  = xTaskGetTickCount();
    const TickType_t frequency = 15000 / portTICK_PERIOD_MS; /**< Fetch API every 1min. */

    while (true) {
        vTaskDelayUntil(&last_wake_time, frequency);

        /**
         * @brief Waiting for WiFi connection.
         *
         */
        EventBits_t sc_bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, false, false, 100 / portTICK_PERIOD_MS);
        if (sc_bits & WIFI_CONNECTED_BIT) {
            while (http.perform() != ESP_OK) {
                vTaskDelay(100 / portTICK_PERIOD_MS); /**< HTTPS fails pretty often. No matter the reason try reconnecting after 100ms. */
            }
        } else {
            ESP_LOGE(TAG, "WiFi disconnected, didn't download API");
        }
    }
    vTaskDelete(nullptr);
}