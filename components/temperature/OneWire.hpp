#pragma once

#include "owb.h"

template <int MaxDevices>
class OneWire {
    OneWireBus *owb_;
    owb_rmt_driver_info rmt_driver_info_{};
    OneWireBus_ROMCode device_rom_codes_[MaxDevices] = {};
    int num_devices_                                 = 0;

        public:
    explicit OneWire(gpio_num_t gpio) {
        owb_ = owb_rmt_initialize(&rmt_driver_info_, gpio, RMT_CHANNEL_1, RMT_CHANNEL_0);
        owb_use_crc(owb_, true); // enable CRC check for ROM code

        OneWireBus_SearchState search_state = {};
        bool found                          = false;
        owb_search_first(owb_, &search_state, &found);
        while (found) {
            char rom_code_s[17];
            owb_string_from_rom_code(search_state.rom_code, rom_code_s, sizeof(rom_code_s));
            printf("  %d : %s\n", num_devices_, rom_code_s);
            device_rom_codes_[num_devices_] = search_state.rom_code;
            num_devices_++;
            owb_search_next(owb_, &search_state, &found);
        }
        printf("Found %d device%s\n", num_devices_, num_devices_ == 1 ? "" : "s");
    }

    int get_num_devices() { return num_devices_; }
    OneWireBus *get_one_wire_bus() { return owb_; }
};
