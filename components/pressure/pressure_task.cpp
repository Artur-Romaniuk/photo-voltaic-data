
#include "BMX280.hpp"
#include "error_state.hpp"

#define BMX280_SAMPLE_DELAY 1000
#define PRESSURE_TAG        "Pressure Task"

void pressure_task(void * /*pvParameters*/) {
    i2c_config_t i2c_cfg = {.mode          = I2C_MODE_MASTER,
                            .sda_io_num    = GPIO_NUM_21,
                            .scl_io_num    = GPIO_NUM_22,
                            .sda_pullup_en = false,
                            .scl_pullup_en = false,
                            .master        = {.clk_speed = 100000}}; // NOLINT

    BMX280 bmx280(&i2c_cfg);

    while (true) {

        bmx280.start_conversion();
        ESP_LOGI(PRESSURE_TAG, "Read Values: pres = %d", bmx280.get_value());
        vTaskDelay(BMX280_SAMPLE_DELAY / portTICK_RATE_MS);
    }

    vTaskDelete(nullptr);
}