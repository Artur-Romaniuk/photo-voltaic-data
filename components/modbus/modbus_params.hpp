#pragma once

#include "esp_modbus_common.h"
#include <cstdint>

constexpr const size_t kMaxNumberOfThermometersModbus     = 3;
constexpr const size_t kMaxNumberOfWeatherForecastsModbus = 4;

#pragma pack(push, 1)
/**
 * @brief Structure for holding weather forecasts.
 *
 */
struct WeatherForecast {
    uint32_t date;
    uint16_t temp;
    uint16_t clouds;
    uint16_t visibility;
    uint16_t humidity;
    uint16_t pressure;
};

#pragma pack(pop)

#pragma pack(push, 1)
struct InputRegParams {
    uint16_t temp[kMaxNumberOfThermometersModbus];
    uint16_t light;
    uint16_t pressure;
    uint16_t humidity;
    WeatherForecast forecasts[kMaxNumberOfWeatherForecastsModbus];
};

#pragma pack(pop)

void update_modbus_temperature(uint8_t iterator, uint16_t value);
void update_modbus_light(uint16_t value);
void update_modbus_pressure(uint16_t value);
void update_modbus_humidity(uint16_t value);
void update_modbus_weather_forecast(uint8_t iterator, const WeatherForecast &value);

extern InputRegParams input_reg_params;
