#include "smartconfig.hpp"

#include "esp_log.h"
#include "esp_smartconfig.h"
#include "wifi_init.hpp"

static const char *TAG = "SmartConfig";
EventBits_t sc_bits; // keeps track of SmartConfig progress

void smartconfig_task(void *parm) {
    smartconfig_start();
    while (1) {
        sc_bits = xEventGroupWaitBits(s_wifi_event_group, SC_ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY);
        if (sc_bits & SC_ESPTOUCH_DONE_BIT) {
            smartconfig_stop();
            ESP_LOGI(TAG, "Deleting smartconfig_task");
            vTaskDelete(NULL);
        }
    }
}

void smartconfig_start() {
    xEventGroupSetBits(s_wifi_event_group, SC_STARTED);
    sc_bits = 0;
    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));
}

void smartconfig_stop() {
    ESP_LOGI(TAG, "smartconfig over");
    xEventGroupClearBits(s_wifi_event_group, SC_STARTED);
    ESP_ERROR_CHECK(esp_smartconfig_stop());
}