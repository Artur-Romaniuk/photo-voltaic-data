#include "DS18B20.hpp"
#include "OneWire.hpp"
#include "error_state.hpp"
#include <cstdio>
#include <vector>

#define GPIO_DS18B20_0     (GPIO_NUM_4)
#define MAX_DEVICES        (2)
#define DS18B20_RESOLUTION (DS18B20_RESOLUTION_12_BIT)
#define SAMPLE_PERIOD      (1000) // milliseconds

#define THERMOMETER_TAG "Thermometer task"

void thermometer_task(void * /*pvParameters*/) {
    OneWire<MAX_DEVICES> one_wire(GPIO_DS18B20_0);

    // Create DS18B20 devices on the 1-Wire bus
    std::vector<DS18B20> devices;
    devices.reserve(one_wire.get_num_devices());
    for (int i = 0; i < one_wire.get_num_devices(); i++) {
        devices.emplace_back(one_wire.get_one_wire_bus(), one_wire.get_device_rom_codes().at(i), DS18B20_RESOLUTION);
    }

    // Read temperatures more efficiently by starting conversions on all devices at the same time
    if (one_wire.get_num_devices() > 0) {
        TickType_t last_wake_time = xTaskGetTickCount();

        while (true) {
            devices[0].convert_all_on_bus();
            devices[0].wait_for_conversion();

            // Read the results immediately after conversion otherwise it may fail
            // (using printf before reading may take too long)
            std::vector<uint16_t> readings(MAX_DEVICES);

            for (int i = 0; i < one_wire.get_num_devices(); ++i) {
                devices[i].start_conversion();
                if (devices[i].get_state() == device_state::DEVICE_OK) {
                    uint16_t temp = devices[i].get_value();
                    readings.push_back(temp);
                    ESP_LOGI(THERMOMETER_TAG, "  %d: %d\n", i, temp);
                } else {
                    ESP_LOGE(THERMOMETER_TAG, "Error on thermometer no. %d", i);
                }
            }

            vTaskDelayUntil(&last_wake_time, SAMPLE_PERIOD / portTICK_PERIOD_MS);
        }
    } else {
        ESP_LOGE(THERMOMETER_TAG, "\nNo DS18B20 devices detected!\n");
    }
    vTaskDelete(nullptr);
}