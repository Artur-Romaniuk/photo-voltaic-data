
#include "bmx280.h"
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

    if (i2c_param_config(I2C_NUM_0, &i2c_cfg) != ESP_OK) {
        ESP_LOGE(PRESSURE_TAG, "I2C config for BMX280 failed");
        vTaskDelete(nullptr);
        return;
    }
    if (i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0) != ESP_OK) {
        ESP_LOGE(PRESSURE_TAG, "I2C installation for BMX280 failed");
        vTaskDelete(nullptr);
        return;
    }
    bmx280_t *bmx280 = bmx280_create(I2C_NUM_0);

    if (!bmx280) {
        ESP_LOGE("test", "Could not create bmx280 driver.");
        bmx280_close(bmx280);
        vTaskDelete(nullptr);
        return;
    }

    if (bmx280_init(bmx280) != ESP_OK) {
        ESP_LOGE(PRESSURE_TAG, "BMX280 init failed");
        bmx280_close(bmx280);
        vTaskDelete(nullptr);
        return;
    }

    bmx280_config_t bmx_cfg = BMX280_DEFAULT_CONFIG;
    if (bmx280_configure(bmx280, &bmx_cfg) != ESP_OK) {
        ESP_LOGE(PRESSURE_TAG, "BMX280 config failed");
        bmx280_close(bmx280);
        vTaskDelete(nullptr);
        return;
    }

    while (true) {

        if (bmx280_setMode(bmx280, BMX280_MODE_FORCE) != ESP_OK) {
            ESP_LOGE(PRESSURE_TAG, "BMX280 set mode failed");
        } else {
            do {
                vTaskDelay(pdMS_TO_TICKS(1));
            } while (bmx280_isSampling(bmx280));

            float temp = 0;
            float pres = 0;
            if (bmx280_readoutFloat(bmx280, &temp, &pres, nullptr) != ESP_OK) {
                ESP_LOGE(PRESSURE_TAG, "BMX280 data read failed");
            } else {
                ESP_LOGI("test", "Read Values: temp = %f, pres = %f", temp, pres / 100);
            }
        }
        vTaskDelay(BMX280_SAMPLE_DELAY / portTICK_RATE_MS);
    }
    bmx280_close(bmx280);
    vTaskDelete(nullptr);
}