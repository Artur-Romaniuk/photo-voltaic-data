#pragma once
#include "ds18b20.h"
#include <memory>
class DS18B20 {
    std::unique_ptr<DS18B20_Info> ds18b20_info_;

        public:
    DS18B20(OneWireBus *owb, OneWireBus_ROMCode rom_code, DS18B20_RESOLUTION resolution) {
        ds18b20_info_ = std::make_unique<DS18B20_Info>(); // heap allocation
        ds18b20_init(ds18b20_info_.get(), owb, rom_code); // associate with bus and device
        ds18b20_use_crc(ds18b20_info_.get(), true);       // enable CRC check on all reads
        ds18b20_set_resolution(ds18b20_info_.get(), resolution);
    }
};
