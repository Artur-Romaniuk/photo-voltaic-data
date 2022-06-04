#pragma once
#include "esp_err.h"
#include "esp_tls.h"
#include <esp_crt_bundle.h>

#define WEB_SERVER "api.openweathermap.org"
#define WEB_URL                                                                                                                            \
    "https://api.openweathermap.org/data/2.5/"                                                                                             \
    "forecast?lat=52.22033734890193&lon=21.011019449081086&cnt=4&appid=7105c60544027a9f1fe64cd5cbe78b4b&units=metric"

struct WeatherForecast {
    uint16_t date;
    uint16_t temp;
    uint16_t clouds;
    uint16_t visibility;
    uint16_t humidity;
    uint16_t pressure;
};

#define NUMBER_OF_FORECASTS 3
extern WeatherForecast forecasts[NUMBER_OF_FORECASTS];

class HttpClient {

        public:
    esp_err_t perform();
};
