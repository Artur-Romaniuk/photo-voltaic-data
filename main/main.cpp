/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "DS18B20.hpp"
#include "OneWire.hpp"
#include "driver/gpio.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "sdkconfig.h"
#include <cstdio>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define GPIO_DS18B20_0     (GPIO_NUM_4)
#define MAX_DEVICES        (2)
#define DS18B20_RESOLUTION (DS18B20_RESOLUTION_12_BIT)
#define SAMPLE_PERIOD      (1000) // milliseconds

void main_cpp() {
    vTaskDelay(2000.0 / portTICK_PERIOD_MS);

    OneWireBus *owb;
    owb_rmt_driver_info rmt_driver_info;
    owb = owb_rmt_initialize(&rmt_driver_info, GPIO_DS18B20_0, RMT_CHANNEL_1, RMT_CHANNEL_0);
    owb_use_crc(owb, true); // enable CRC check for ROM code

    // Find all connected devices
    printf("Find devices:\n");
    OneWire<MAX_DEVICES> one_wire(GPIO_DS18B20_0);

    // Create DS18B20 devices on the 1-Wire bus
    std::unique_ptr<DS18B20> devices[MAX_DEVICES] = {};
    for (int i = 0; i < one_wire.get_num_devices(); i++) {
        devices[i] = std::make_unique<DS18B20>(one_wire.get_one_wire_bus(), );
        }


    // Read temperatures more efficiently by starting conversions on all devices at the same time
    int errors_count[MAX_DEVICES] = {};
    int sample_count              = 0;
    if (num_devices > 0) {
        TickType_t last_wake_time = xTaskGetTickCount();

        while (1) {
            ds18b20_convert_all(owb);

            // In this application all devices use the same resolution,
            // so use the first device to determine the delay
            ds18b20_wait_for_conversion(devices[0]);

            // Read the results immediately after conversion otherwise it may fail
            // (using printf before reading may take too long)
            float readings[MAX_DEVICES]       = {};
            DS18B20_ERROR errors[MAX_DEVICES] = {};

            for (int i = 0; i < num_devices; ++i) {
                errors[i] = ds18b20_read_temp(devices[i], &readings[i]);
            }

            // Print results in a separate loop, after all have been read
            printf("\nTemperature readings (degrees C): sample %d\n", ++sample_count);
            for (int i = 0; i < num_devices; ++i) {
                if (errors[i] != DS18B20_OK) {
                    ++errors_count[i];
                }

                printf("  %d: %.1f    %d errors\n", i, readings[i], errors_count[i]);
            }

            vTaskDelayUntil(&last_wake_time, SAMPLE_PERIOD / portTICK_PERIOD_MS);
        }
    } else {
        printf("\nNo DS18B20 devices detected!\n");
    }

    // clean up dynamically allocated data
    for (int i = 0; i < num_devices; ++i) {
        ds18b20_free(&devices[i]);
    }
    owb_uninitialize(owb);

    printf("Restarting now.\n");
    fflush(stdout);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    esp_restart();
}

extern "C" void app_main(void) { main_cpp(); }
