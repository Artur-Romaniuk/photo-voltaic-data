
#include "BMP280.hpp"
#include "DHT11.hpp"
#include "error_state.hpp"

#define PRESSURE_TAG "Pressure Task"
#define HUMIDITY_TAG "Humidity Task"

uint16_t pressure{}; /**< Modbus task will get data from this global variable. */
uint16_t humidity{}; /**< Modbus task will get data from this global variable. */

void pressure_task(void * /*pvParameters*/) {

    BMP280 bmp280;
    DHT11 dht11;
    TickType_t last_wake_time  = xTaskGetTickCount();
    const TickType_t frequency = 2000 / portTICK_PERIOD_MS;
    while (true) {
        vTaskDelayUntil(&last_wake_time, frequency);

        bmp280.start_conversion();
        dht11.start_conversion();
        ESP_LOGI(PRESSURE_TAG, "Read Values: pres = %d", bmp280.get_value());
        ESP_LOGI(HUMIDITY_TAG, "Read Values: hum = %d", dht11.get_value());

        pressure = bmp280.get_value();
        humidity = dht11.get_value();
    }

    vTaskDelete(nullptr);
}