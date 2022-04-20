#pragma once

#include "esp_event.h"

/**
 * @brief Function responsible for handling WiFI, IP and SC events.
 *
 * @param arg
 * @param event_base
 * @param event_id
 * @param event_data
 */
void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);