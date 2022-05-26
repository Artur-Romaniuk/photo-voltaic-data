#include "error_state.hpp"

void error_check(esp_err_t err, const char *error_msg) {
    if (err != ESP_OK) {
        go_error_state(error_msg);
    }
}

void go_error_state(const char *error_msg) {
    for (;;) {
        ESP_LOGE("ERROR STATE", "%s", error_msg);
    }
}