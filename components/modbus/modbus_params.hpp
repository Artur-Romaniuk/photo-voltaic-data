#pragma once

#include "esp_modbus_common.h"
#include <cstdint>

constexpr const size_t kMaxNumberOfThermometersModbus     = 3;
constexpr const size_t kMaxNumberOfWeatherForecastsModbus = 8;

#pragma pack(push, 1)
/**
 * @brief Structure for holding weather forecasts.
 *
 */
struct WeatherForecast {
    uint16_t date_u;
    uint16_t date_l;
    int16_t temp;
    uint16_t clouds;
    uint16_t visibility;
    uint16_t humidity;
    uint16_t pressure;
};

#pragma pack(pop)

#pragma pack(push, 1)
struct InputRegParams {
    int16_t temp[kMaxNumberOfThermometersModbus];
    uint16_t light;
    uint16_t pressure;
    uint16_t humidity;
    WeatherForecast forecasts[kMaxNumberOfWeatherForecastsModbus];
};

#pragma pack(pop)

#pragma pack(push, 1)
struct HoldingRegParams {
    uint16_t latitude_u  = (uint32_t)52220337 >> 16; // default values for warsaw
    uint16_t latitude_l  = (uint16_t)52220337;
    uint16_t longitude_u = (uint32_t)21011019 >> 16; // default values for warsaw
    uint16_t longitude_l = (uint16_t)21011019;
};

#pragma pack(pop)

void update_modbus_temperature(uint8_t iterator, int16_t value);
void update_modbus_light(uint16_t value);
void update_modbus_pressure(uint16_t value);
void update_modbus_humidity(uint16_t value);
void update_modbus_weather_forecast(uint8_t iterator, const WeatherForecast &value);

double read_modbus_latitude();
double read_modbus_longitude();

extern InputRegParams input_reg_params;
extern HoldingRegParams holding_reg_params;
