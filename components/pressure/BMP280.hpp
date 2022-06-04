#pragma once

#include "bmp280.h"
#include "device.hpp"
#include "esp_log.h"
#include "i2c_config.hpp"

#define PRESSURE_DEVICE_TAG "Pressure Task"

class BMP280 : public Device {
    bmp280_t device_struct_{};
    bmp280_params_t params_{};
    uint16_t pressure_ = 0;

        public:
    BMP280() {
        bmp280_init_desc(&device_struct_, BMP280_I2C_ADDRESS_0, I2C_NUM_0, static_cast<gpio_num_t>(i2c_config::kI2cCfg.sda_io_num),
                         static_cast<gpio_num_t>(i2c_config::kI2cCfg.scl_io_num));
        bmp280_init_default_params(&params_);
    }
    ~BMP280() { bmp280_free_desc(&device_struct_); }

    void start_conversion() override {
        bmp280_init(&device_struct_, &params_);
        bool is_busy = true;
        while (is_busy) {
            if (bmp280_is_measuring(&device_struct_, &is_busy) != ESP_OK) {
                vTaskDelay(1000 / portTICK_RATE_MS);
            }
            vTaskDelay(15 / portTICK_RATE_MS); // for standard resolution measurement should take up to 13ms. 15ms is guaranteed.
        }
        int32_t temperature{};
        uint32_t pressure{};
        if (ESP_OK == bmp280_read_fixed(&device_struct_, &temperature, &pressure, nullptr)) {
            pressure_ = pressure / 256 / 100;
        }
    }

    uint16_t get_value() override { return pressure_; }
};