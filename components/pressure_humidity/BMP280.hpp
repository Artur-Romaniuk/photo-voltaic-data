#pragma once

#include "bmp280.h"
#include "device.hpp"
#include "i2c_config.hpp"

class BMP280 : public Device {
    bmp280_t device_struct_{};
    bmp280_params_t params_{.mode                     = BMP280_MODE_FORCED,
                            .filter                   = BMP280_FILTER_4,
                            .oversampling_pressure    = BMP280_STANDARD,
                            .oversampling_temperature = BMP280_SKIPPED,
                            .oversampling_humidity    = BMP280_SKIPPED,
                            .standby                  = BMP280_STANDBY_05

    };
    uint16_t pressure_ = 0;

        public:
    BMP280() {
        i2c_config::i2c_init();
        bmp280_init_desc(&device_struct_, BMP280_I2C_ADDRESS_0, I2C_NUM_0, static_cast<gpio_num_t>(i2c_config::kI2cCfg.sda_io_num),
                         static_cast<gpio_num_t>(i2c_config::kI2cCfg.scl_io_num));
    }
    ~BMP280() { bmp280_free_desc(&device_struct_); }
    BMP280(const BMP280 &other) = delete;

    void start_conversion() override {
        if (bmp280_init(&device_struct_, &params_) != ESP_OK) {
            this->set_state(device_state::DEVICE_INIT_ERROR);
            return;
        }
        if (bmp280_force_measurement(&device_struct_) != ESP_OK) {
            this->set_state(device_state::DEVICE_ERROR);
            return;
        }
        bool is_busy = true;
        while (is_busy) {
            if (bmp280_is_measuring(&device_struct_, &is_busy) != ESP_OK) {
                this->set_state(device_state::DEVICE_ERROR);
                return;
            }
            vTaskDelay(15 / portTICK_RATE_MS); /**< for standard resolution measurement should take up to 13ms. 15ms is guaranteed. */
        }
        int32_t temperature{};
        uint32_t pressure{};
        if (bmp280_read_fixed(&device_struct_, &temperature, &pressure, nullptr) != ESP_OK) {
            this->set_state(device_state::DEVICE_ERROR);
            return;
        }
        pressure_ = pressure / 256 / 100; /**< Convertion to hPa */
    }

    uint16_t get_value() override { return pressure_; }
};