#pragma once
#include "device.hpp"
#include "ds18x20.h"
#include "esp_log.h"
#include <vector>

class DS18B20 : public Device {
    gpio_num_t pin_{};
    static constexpr const size_t kMaxNumberOfDevices = 5;
    ds18x20_addr_t addr_list_[kMaxNumberOfDevices];
    std::vector<uint16_t> readings_{};
    size_t number_of_devices_{};

        public:
    DS18B20(gpio_num_t pin) : pin_{pin} {
        ds18x20_scan_devices(pin, addr_list_, kMaxNumberOfDevices, &number_of_devices_);
        readings_.resize(number_of_devices_);
    }

    void start_conversion() override {
        ds18x20_measure(pin_, DS18X20_ANY, true);
        float buffer[kMaxNumberOfDevices]{};
        ds18x20_read_temp_multi(pin_, addr_list_, number_of_devices_, buffer);
        ESP_LOGI("TEST", "%zu", number_of_devices_);
        for (size_t i = 0; i < number_of_devices_; i++) {
            readings_[i] = buffer[i] * 100;
        }
    }

    uint16_t get_value() override {
        uint16_t sum = 0;
        for (size_t i = 0; i < number_of_devices_; i++) {
            sum += readings_[i];
        }
        return sum / number_of_devices_;
    }

    std::vector<uint16_t> get_vector() {
        ESP_LOGI("TEST", "%zu", readings_.size());
        return readings_;
    }
};
