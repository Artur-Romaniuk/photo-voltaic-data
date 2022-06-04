#pragma once
#include "esp_err.h"
#include "esp_tls.h"
#include <esp_crt_bundle.h>


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
