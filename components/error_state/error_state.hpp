#pragma once

#include "esp_err.h"
#include "esp_log.h"

void go_error_state(const char *error_msg);
void error_check(esp_err_t err, const char *error_msg);