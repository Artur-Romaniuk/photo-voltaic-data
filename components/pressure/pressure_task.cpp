
#include "BMX280.hpp"
#include "error_state.hpp"

#define BMX280_SAMPLE_DELAY 1000
#define PRESSURE_TAG        "Pressure Task"

void pressure_task(void * /*pvParameters*/) {

    BMX280 bmx280;

    while (true) {

        bmx280.start_conversion();
        ESP_LOGI(PRESSURE_TAG, "Read Values: pres = %d", bmx280.get_value());
        vTaskDelay(BMX280_SAMPLE_DELAY / portTICK_RATE_MS);
    }

    vTaskDelete(nullptr);
}