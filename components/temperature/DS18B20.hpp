#pragma once
#include "device.hpp"
#include "ds18b20.h"
#include <memory>
#include <variant>

class DS18B20 : public Device {
    OneWireBus *owb_;
    DS18B20_Info ds18b20_info_{};
    float buffer_ = 0;

        public:
    DS18B20(OneWireBus *owb, OneWireBus_ROMCode rom_code, DS18B20_RESOLUTION resolution) : owb_(owb) {
        ds18b20_init(&ds18b20_info_, owb_, rom_code); // associate with bus and device
        ds18b20_use_crc(&ds18b20_info_, true);        // enable CRC check on all reads
        ds18b20_set_resolution(&ds18b20_info_, resolution);
    }

    void wait_for_conversion() { ds18b20_wait_for_conversion(&ds18b20_info_); }

    void start_conversion() override {
        DS18B20_ERROR err = ds18b20_read_temp(&ds18b20_info_, &buffer_);
        if (err != DS18B20_OK) {
            set_state(device_state::DEVICE_CONNECTION_ERROR);
        }
        set_state(device_state::DEVICE_OK);
    }

    void convert_all_on_bus() { ds18b20_convert_all(owb_); }

    uint16_t get_value() override { return static_cast<uint16_t>(buffer_); }
};
