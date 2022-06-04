
#include "BMP280.hpp"
#include "error_state.hpp"

#define PRESSURE_TAG "Pressure Task"

uint16_t pressure{};

void pressure_task(void * /*pvParameters*/) {

    BMP280 bmp280;
    TickType_t last_wake_time  = xTaskGetTickCount();
    const TickType_t frequency = 2000 / portTICK_PERIOD_MS;
    while (true) {
        vTaskDelayUntil(&last_wake_time, frequency);

        bmp280.start_conversion();
        ESP_LOGI(PRESSURE_TAG, "Read Values: pres = %d", bmp280.get_value());
        pressure = bmp280.get_value();
    }

    vTaskDelete(nullptr);
}