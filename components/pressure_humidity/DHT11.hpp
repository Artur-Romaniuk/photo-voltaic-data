#pragma once

#include "device.hpp"
#include "dht.h"
#include "esp_err.h"
#include "esp_log.h"

constexpr gpio_num_t kDht11GpioNum = GPIO_NUM_5;

class DHT11 : public Device {
    uint16_t humidity_ = 0;

        public:
    void start_conversion() override {
        int16_t humidity    = 0;
        int16_t temperature = 0;
        if (dht_read_data(DHT_TYPE_DHT11, kDht11GpioNum, &humidity, &temperature) != ESP_OK)
            this->set_state(device_state::DEVICE_ERROR);
        else {
            this->set_state(device_state::DEVICE_OK);
            humidity_ = humidity;
        };
    }

    uint16_t get_value() override { return humidity_; }
};