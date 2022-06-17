/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "HttpClient.hpp"
#include "esp_crt_bundle.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "http_task.hpp"
#include "i2c_config.hpp"
#include "light_task.hpp"
#include "modbus_task.hpp"
#include "nvs_flash.h"
#include "pressure_humidity_task.hpp"
#include "sdkconfig.h"
#include "thermometer_task.hpp"
#include "wifi_init.hpp"
#include <cstdio>
#include <vector>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "main"

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

    /**
     * @brief Starting wifi initialization sequence.
     *
     */

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_sta_start();
    vTaskDelay(5000 / portTICK_RATE_MS);

    /**
     * @brief Starting all component tasks.
     *
     */
    TaskHandle_t thermometer_task_handle = nullptr;
    xTaskCreate(thermometer_task, "thermometer", 2400, nullptr, 5, &thermometer_task_handle);
    TaskHandle_t pressure_humidity_task_handle = nullptr;
    xTaskCreate(pressure_humidity_task, "pressure", 2400, nullptr, 5, &pressure_humidity_task_handle);
    TaskHandle_t light_task_handle = nullptr;
    xTaskCreate(light_task, "light", 2400, nullptr, 5, &light_task_handle);
    TaskHandle_t http_task_handle = nullptr;
    xTaskCreate(http_task, "http", 7500, nullptr, 5, &http_task_handle);
    TaskHandle_t modbus_task_handle = nullptr;
    xTaskCreate(modbus_task, "modbus", 5000, nullptr, 5, &modbus_task_handle);

    /**
     * @brief Main task is printing free stack space for debug.
     *
     */
    while (true) {
        UBaseType_t uxHighWaterMark;
        uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        ESP_LOGI(TAG, "Main: %d", (int)uxHighWaterMark);
        uxHighWaterMark = uxTaskGetStackHighWaterMark(modbus_task_handle);
        ESP_LOGI(TAG, "Modbus: %d", (int)uxHighWaterMark);
        uxHighWaterMark = uxTaskGetStackHighWaterMark(thermometer_task_handle);
        ESP_LOGI(TAG, "Thermometer: %d", (int)uxHighWaterMark);
        uxHighWaterMark = uxTaskGetStackHighWaterMark(pressure_humidity_task_handle);
        ESP_LOGI(TAG, "Pressure: %d", (int)uxHighWaterMark);
        uxHighWaterMark = uxTaskGetStackHighWaterMark(light_task_handle);
        ESP_LOGI(TAG, "Light: %d", (int)uxHighWaterMark);
        uxHighWaterMark = uxTaskGetStackHighWaterMark(http_task_handle);
        ESP_LOGI(TAG, "HTTP: %d", (int)uxHighWaterMark);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    esp_restart();
}

extern "C" void app_main(void) { main_cpp(); }
