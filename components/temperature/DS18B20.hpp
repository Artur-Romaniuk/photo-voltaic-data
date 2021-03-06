#pragma once
#include "device.hpp"
#include "ds18x20.h"
#include "esp_log.h"
#include <vector>

constexpr const size_t kMaxNumberOfDevices = 3;

class DS18B20 : public Device {
    gpio_num_t pin_{};
    ds18x20_addr_t addr_list_[kMaxNumberOfDevices];
    std::vector<int16_t> readings_{};
    size_t number_of_devices_{};

        public:
    DS18B20(gpio_num_t pin) : pin_{pin} {}

    void start_conversion() override {
        if (ds18x20_scan_devices(pin_, addr_list_, kMaxNumberOfDevices, &number_of_devices_) != ESP_OK) {
            this->set_state(device_state::DEVICE_INIT_ERROR);
            return;
        }
        readings_.resize(number_of_devices_);
        if (ds18x20_measure(pin_, DS18X20_ANY, true) != ESP_OK) {
            this->set_state(device_state::DEVICE_ERROR);
            return;
        }
        float buffer[kMaxNumberOfDevices]{};
        if (ds18x20_read_temp_multi(pin_, addr_list_, number_of_devices_, buffer) != ESP_OK) {
            this->set_state(device_state::DEVICE_ERROR);
            return;
        }
        for (size_t i = 0; i < number_of_devices_; i++) {
            readings_[i] = buffer[i] * 100;
        }
    }

    uint16_t get_value() override { return 0; }

    std::vector<int16_t> get_vector() { return readings_; }
};
