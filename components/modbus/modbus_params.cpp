#include "modbus_params.hpp"

#include "esp_log.h"

portMUX_TYPE param_lock = portMUX_INITIALIZER_UNLOCKED;
InputRegParams input_reg_params{};
HoldingRegParams holding_reg_params{};

static void update_modbus_data(uint16_t *data_ptr, uint16_t value) {
    portENTER_CRITICAL(&param_lock);
    *data_ptr = value;
    portEXIT_CRITICAL(&param_lock);
}
void update_modbus_weather_forecast(uint8_t iterator, const WeatherForecast &value) {
    if (iterator >= kMaxNumberOfWeatherForecastsModbus) {
        return;
    }
    portENTER_CRITICAL(&param_lock);
    input_reg_params.forecasts[iterator] = value;
    portEXIT_CRITICAL(&param_lock);
}

void update_modbus_temperature(uint8_t iterator, int16_t value) {
    if (iterator >= kMaxNumberOfThermometersModbus) {
        return;
    }
    portENTER_CRITICAL(&param_lock);
    input_reg_params.temp[iterator] = value;
    portEXIT_CRITICAL(&param_lock);
};
void update_modbus_light(uint16_t value) { update_modbus_data(&input_reg_params.light, value); }
void update_modbus_pressure(uint16_t value) { update_modbus_data(&input_reg_params.pressure, value); }
void update_modbus_humidity(uint16_t value) { update_modbus_data(&input_reg_params.humidity, value); }

double read_modbus_latitude() {
    portENTER_CRITICAL(&param_lock);
    double val = ((holding_reg_params.latitude_u << 16) + holding_reg_params.latitude_l) / 1.e+6;
    portEXIT_CRITICAL(&param_lock);
    return val;
}

double read_modbus_longitude() {
    portENTER_CRITICAL(&param_lock);
    double val = ((holding_reg_params.longitude_u << 16) + holding_reg_params.longitude_l) / 1.e+6;
    portEXIT_CRITICAL(&param_lock);
    return val;
}
