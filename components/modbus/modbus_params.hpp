#pragma once

#include <cstdint>

#pragma pack(push, 1)
struct InputRegParams {
    uint16_t temp_r1;
    uint16_t temp_r2;
    uint16_t temp_r3;
    uint16_t light;
    uint16_t pressure;
    uint16_t humidity;
    uint16_t fc1_date;
    uint16_t fc1_temp;
    uint16_t fc1_clouds;
    uint16_t fc1_visibility;
    uint16_t fc1_humidity;
    uint16_t fc1_pressure;
    uint16_t fc2_date;
    uint16_t fc2_temp;
    uint16_t fc2_clouds;
    uint16_t fc2_visibility;
    uint16_t fc2_humidity;
    uint16_t fc2_pressure;
    uint16_t fc3_date;
    uint16_t fc3_temp;
    uint16_t fc3_clouds;
    uint16_t fc3_visibility;
    uint16_t fc3_humidity;
    uint16_t fc3_pressure;
};
#pragma pack(pop)

extern InputRegParams input_reg_params;
