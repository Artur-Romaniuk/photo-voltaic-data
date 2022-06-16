#include "thermometer_task.hpp"

#include "modbus_params.hpp"
#include <cstdio>
#include <vector>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

constexpr gpio_num_t kGpioDS18B20 = GPIO_NUM_4;

#define THERMOMETER_TAG "Thermometer task"

static_assert(kMaxNumberOfDevices <= kMaxNumberOfThermometersModbus); // asserting that Modbus struct has more than enough places for temperature readings

void thermometer_task(void * /*pvParameters*/) {

    DS18B20 ds18b20(kGpioDS18B20);
    TickType_t last_wake_time  = xTaskGetTickCount();
    const TickType_t frequency = 2000 / portTICK_PERIOD_MS;
    while (true) {
        vTaskDelayUntil(&last_wake_time, frequency);

        ds18b20.start_conversion();
        for (size_t i = 0; i < ds18b20.get_vector().size(); i++) {
            ESP_LOGI(THERMOMETER_TAG, "Temperature no. %zu: %d", i, ds18b20.get_vector().at(i));
            update_modbus_temperature(i, ds18b20.get_vector().at(i));
        }
    }
    vTaskDelete(nullptr);
}