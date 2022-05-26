/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "HttpClient.hpp"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "i2c_config.hpp"
#include "light_task.hpp"
#include "nvs_flash.h"
#include "pressure_task.hpp"
#include "sdkconfig.h"
#include "thermometer_task.hpp"
#include "wifi_init.hpp"
#include <cstdio>
#include <vector>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "http_client"

esp_err_t _http_event_handle(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
        printf("%.*s", evt->data_len, (char *)evt->data);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        if (!esp_http_client_is_chunked_response(evt->client)) {
            printf("%.*s", evt->data_len, (char *)evt->data);
        }

        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    }
    return ESP_OK;
}

void main_cpp() {

    /**
     * @brief Initializing NVS
     *
     */
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_sta_start();
    vTaskDelay(5000 / portTICK_RATE_MS);

    i2c_config::i2c_init();
    TaskHandle_t thermometer_task_handle = nullptr;
    xTaskCreate(thermometer_task, "thermometer", 2048, nullptr, 5, &thermometer_task_handle);
    // TaskHandle_t pressure_task_handle = nullptr;
    // xTaskCreate(pressure_task, "pressure", 2048, nullptr, 5, &pressure_task_handle);
    TaskHandle_t light_task_handle = nullptr;
    xTaskCreate(light_task, "light", 2048, nullptr, 5, &light_task_handle);

    esp_http_client_config_t config = {
        .url           = "http://universities.hipolabs.com/search?country=Poland",
        .event_handler = _http_event_handle,
    };
    HttpClient http(config);

    while (true) {
        http.perform();
        vTaskDelay(10000 / portTICK_RATE_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    esp_restart();
}

extern "C" void app_main(void) { main_cpp(); }
