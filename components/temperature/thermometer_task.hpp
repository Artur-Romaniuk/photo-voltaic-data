#pragma once
#include "DS18B20.hpp"

void thermometer_task(void *pvParameters);
extern uint16_t temperatures[kMaxNumberOfDevices];