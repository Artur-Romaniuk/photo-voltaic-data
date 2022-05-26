#include "BH1750.hpp"

#define LIGHT_TAG "Light Task"

void light_task(void * /*pvParameters*/) {
    BH1750 bh1750;
    while (true) {
        bh1750.start_conversion();
        ESP_LOGI(LIGHT_TAG, "Read Values: light = %d", bh1750.get_value());
        bh1750.get_value();
        vTaskDelay(200 / portTICK_RATE_MS);
    }
}