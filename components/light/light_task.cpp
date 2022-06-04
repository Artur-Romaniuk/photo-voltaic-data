#include "BH1750.hpp"

#define LIGHT_TAG "Light Task"

uint16_t light{}; /**< Modbus task will get data from this global variable. */

void light_task(void * /*pvParameters*/) {
    BH1750 bh1750;
    TickType_t last_wake_time  = xTaskGetTickCount();
    const TickType_t frequency = 2000 / portTICK_PERIOD_MS;
    while (true) {
        vTaskDelayUntil(&last_wake_time, frequency);

        bh1750.start_conversion();
        ESP_LOGI(LIGHT_TAG, "Read Values: light = %d", bh1750.get_value());
        light = bh1750.get_value();
    }
}