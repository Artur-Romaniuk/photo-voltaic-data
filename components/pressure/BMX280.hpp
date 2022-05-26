#pragma once

#include "bmx280.h"
#include "device.hpp"
#include "esp_log.h"

#define PRESSURE_DEVICE_TAG "Pressure Task"

class BMX280 : public Device {
    bmx280_t *bmx280;
    int32_t temp_  = 0;
    uint32_t pres_ = 0;

        public:
    BMX280(i2c_config_t *i2c_config) {
        if (i2c_param_config(I2C_NUM_0, i2c_config) != ESP_OK) {
            ESP_LOGE(PRESSURE_DEVICE_TAG, "I2C config for BMX280 failed");
            set_state(device_state::DEVICE_INIT_ERROR);
            return;
        }
        if (i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0) != ESP_OK) {
            ESP_LOGE(PRESSURE_DEVICE_TAG, "I2C installation for BMX280 failed");
            set_state(device_state::DEVICE_INIT_ERROR);
            return;
        }
        bmx280 = bmx280_create(I2C_NUM_0);
        if (!bmx280) {
            ESP_LOGE(PRESSURE_DEVICE_TAG, "Could not create bmx280 driver.");
            set_state(device_state::DEVICE_INIT_ERROR);
            return;
        }
        if (bmx280_init(bmx280) != ESP_OK) {
            ESP_LOGE(PRESSURE_DEVICE_TAG, "BMX280 init failed");
            set_state(device_state::DEVICE_INIT_ERROR);
            return;
        }
        bmx280_config_t bmx_cfg = BMX280_DEFAULT_CONFIG;
        if (bmx280_configure(bmx280, &bmx_cfg) != ESP_OK) {
            ESP_LOGE(PRESSURE_DEVICE_TAG, "BMX280 config failed");
            set_state(device_state::DEVICE_INIT_ERROR);
            return;
        }
    }
    ~BMX280() { bmx280_close(bmx280); }

    void start_conversion() override {
        if (get_state() == device_state::DEVICE_INIT_ERROR) {
            return;
        }
        if (bmx280_setMode(bmx280, BMX280_MODE_FORCE) != ESP_OK) {
            ESP_LOGE(PRESSURE_DEVICE_TAG, "BMX280 set mode failed");
            set_state(device_state::DEVICE_CONNECTION_ERROR);
            return;
        }

        do {
            vTaskDelay(pdMS_TO_TICKS(1));
        } while (bmx280_isSampling(bmx280));

        if (bmx280_readout(bmx280, &temp_, &pres_, nullptr) != ESP_OK) {
            ESP_LOGE(PRESSURE_DEVICE_TAG, "BMX280 data read failed");
            set_state(device_state::DEVICE_CONNECTION_ERROR);
        }
    }

    uint16_t get_value() override { return static_cast<uint16_t>(pres_ / 256 / 100); }
};