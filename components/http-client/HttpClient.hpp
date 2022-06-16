#pragma once
#include "esp_err.h"
#include "esp_tls.h"
#include <esp_crt_bundle.h>

class HttpClient {

        public:
    esp_err_t perform();
};
