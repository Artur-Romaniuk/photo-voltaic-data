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
    // Find all connected devices
    ESP_LOGI(THERMOMETER_TAG, "Find devices:\n");
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
            std::vector<float> readings(MAX_DEVICES);
            std::vector<DS18B20_ERROR> errors(MAX_DEVICES);

            for (int i = 0; i < one_wire.get_num_devices(); ++i) {
                std::variant<float, DS18B20_ERROR> read_temp_result = devices[i].read_temperature();
                if (const float *temp = std::get_if<float>(&read_temp_result)) {
                    readings.push_back(*temp);
                    ESP_LOGI(THERMOMETER_TAG, "  %d: %.1f\n", i, *temp);
                } else {
                    ESP_LOGE(THERMOMETER_TAG, "Error %d on thermometer no. %d", std::get<DS18B20_ERROR>(read_temp_result), i);
                }
            }

            vTaskDelayUntil(&last_wake_time, SAMPLE_PERIOD / portTICK_PERIOD_MS);
        }
    } else {
        ESP_LOGE(THERMOMETER_TAG, "\nNo DS18B20 devices detected!\n");
    }
    vTaskDelete(nullptr);
}