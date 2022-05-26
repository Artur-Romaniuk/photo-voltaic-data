
#include "BMP280.hpp"
#include "error_state.hpp"

#define BMX280_SAMPLE_DELAY 4000
#define PRESSURE_TAG        "Pressure Task"

void pressure_task(void * /*pvParameters*/) {

    BMP280 bmp280;

    while (true) {

        bmp280.start_conversion();
        ESP_LOGI(PRESSURE_TAG, "Read Values: pres = %d", bmp280.get_value());
        vTaskDelay(BMX280_SAMPLE_DELAY / portTICK_RATE_MS);
    }

    vTaskDelete(nullptr);
}