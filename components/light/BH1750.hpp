#pragma once
#include "bh1750.h"
#include "device.hpp"
#include "i2c_config.hpp"

#define BH1750_TAG "BH1750"

class BH1750 : public Device {
    i2c_dev_t device_struct_{};
    uint16_t buffer_ = 0;

        public:
    explicit BH1750() {
        i2c_config::i2c_init();
        if (bh1750_init_desc(&device_struct_, BH1750_ADDR_LO, I2C_NUM_0, static_cast<gpio_num_t>(i2c_config::kI2cCfg.sda_io_num),
                             static_cast<gpio_num_t>(i2c_config::kI2cCfg.scl_io_num)) != ESP_OK) {
            ESP_LOGE(BH1750_TAG, "BH1750 init desc error");
        }
        ESP_LOGI(BH1750_TAG, "%d", device_struct_.port);
        init();
    }
    ~BH1750() { bh1750_free_desc(&device_struct_); }

    void init() {
        if (bh1750_power_on(&device_struct_) != ESP_OK) {
            ESP_LOGE(BH1750_TAG, "BH1750 power on error");
            this->set_state(device_state::DEVICE_CONNECTION_ERROR);
            return;
        }
        if (bh1750_setup(&device_struct_, BH1750_MODE_CONTINUOUS, BH1750_RES_HIGH)) {
            ESP_LOGE(BH1750_TAG, "BH1750 setup error");
            this->set_state(device_state::DEVICE_INIT_ERROR);
            return;
        }
        this->set_state(device_state::DEVICE_OK);
    }

    void start_conversion() override {
        vTaskDelay(120 / portTICK_RATE_MS);
        if (bh1750_read(&device_struct_, &buffer_) != ESP_OK) {
            this->set_state(device_state::DEVICE_ERROR);
            init();
        }
    }

    uint16_t get_value() override {
        uint32_t calc_buffer = buffer_;
        return calc_buffer * 1000 / 685; // convert to mW/m^2
    }
};