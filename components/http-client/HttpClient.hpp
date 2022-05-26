#pragma once

#include "esp_http_client.h"

class HttpClient {
    esp_http_client_handle_t client_;

        public:
    explicit HttpClient(esp_http_client_config_t config) : client_{esp_http_client_init(&config)} {}
    ~HttpClient() { esp_http_client_cleanup(client_); }
    esp_err_t perform() { return esp_http_client_perform(client_); }
};