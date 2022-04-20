#pragma once
#include "ds18b20.h"
#include <memory>
#include <variant>

class DS18B20 {
    OneWireBus *owb_;
    DS18B20_Info ds18b20_info_{};

        public:
    DS18B20(OneWireBus *owb, OneWireBus_ROMCode rom_code, DS18B20_RESOLUTION resolution) : owb_(owb) {
        ds18b20_init(&ds18b20_info_, owb_, rom_code); // associate with bus and device
        ds18b20_use_crc(&ds18b20_info_, true);        // enable CRC check on all reads
        ds18b20_set_resolution(&ds18b20_info_, resolution);
    }

    void wait_for_conversion() { ds18b20_wait_for_conversion(&ds18b20_info_); }

    std::variant<float, DS18B20_ERROR> read_temperature() {
        float result{};
        DS18B20_ERROR err = ds18b20_read_temp(&ds18b20_info_, &result);
        if (err != DS18B20_OK) {
            return err;
        }
        return result;
    }

    void convert_all_on_bus() { ds18b20_convert_all(owb_); }
};
