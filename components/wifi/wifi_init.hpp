#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#define WIFI_CONNECTED_BIT   BIT0
#define WIFI_FAIL_BIT        BIT1
#define SC_ESPTOUCH_DONE_BIT BIT2
#define SC_STARTED           BIT3

extern EventGroupHandle_t s_wifi_event_group;

/**
 * @brief Starts wifi connection procedure.
 *
 */
void wifi_sta_start(void);