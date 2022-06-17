#include "web_server_config.hpp"

#include "modbus_params.hpp"
#include "web_server_appid.hpp"

constexpr const char WEB_SERVER[] = "api.openweathermap.org";
constexpr const char WEB_URL[]    = "https://api.openweathermap.org/data/2.5/forecast?&units=metric&";
constexpr uint16_t count          = kMaxNumberOfWeatherForecastsModbus;

std::string get_url() {
    std::string url = WEB_URL;
    url += "lat=" + std::to_string(read_modbus_latitude()) + "&lon=" + std::to_string(read_modbus_longitude()) +
           "&cnt=" + std::to_string(count) + "&appid=" + appid;
    return url;
}

std::string get_request() {
    std::string request = "GET ";
    request += get_url() + " HTTP/1.1\r\n";
    request += "Host: ";
    request += WEB_SERVER;
    request += "\r\n"
               "User-Agent: esp-idf/1.0 esp32\r\n"
               "\r\n";
    return request;
}