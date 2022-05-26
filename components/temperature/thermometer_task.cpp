#include "DS18B20.hpp"
#include "error_state.hpp"
#include <cstdio>
#include <vector>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define GPIO_DS18B20_0     (GPIO_NUM_4)

#define THERMOMETER_TAG "Thermometer task"

void thermometer_task(void * /*pvParameters*/) {

    DS18B20 ds18b20(GPIO_DS18B20_0);
    while (true) {
        ds18b20.start_conversion();
        std::vector<uint16_t> readings = ds18b20.get_vector();
        ESP_LOGI(THERMOMETER_TAG, "%zu", readings.size());
        for (size_t i = 0; i < readings.size(); i++) {
            ESP_LOGI(THERMOMETER_TAG, "Temperature no. %zu: %d", i, readings[i]);
        }
    }
    vTaskDelete(nullptr);
}