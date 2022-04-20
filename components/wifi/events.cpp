#include "events.hpp"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_smartconfig.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "nvs_controller.hpp"
#include "smartconfig.hpp"
#include "wifi_init.hpp"
#include <cstring>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

static const char *TAG = "WiFi event";

int s_retry_num           = 0;
const int s_maximum_retry = 0;

void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT) {
        if (event_id == WIFI_EVENT_STA_START) {
            uint8_t ssid[33]     = {0};
            uint8_t password[65] = {0};
            esp_err_t err        = read_credentials_nvs(ssid, password); // check if there are saved credentials
            if (err != ESP_OK)                                           // if not start SmartConfig
            {
                ESP_LOGI(TAG, "Creating smartconfig_task");
                xTaskCreate(smartconfig_task, "smartconfig_task", 4096, NULL, 3, NULL);
            } else {
                wifi_config_t wifi_config;
                memset(&wifi_config, 0, sizeof(wifi_config_t));
                memcpy(wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
                memcpy(wifi_config.sta.password, password, sizeof(wifi_config.sta.password));
                ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
                esp_wifi_connect();
            }
        } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
            if (s_retry_num < s_maximum_retry) {
                esp_wifi_connect();
                s_retry_num++;
                ESP_LOGI(TAG, "retry to connect to the AP");
            } else {
                xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            }
            ESP_LOGI(TAG, "connect to the AP fail");
            erase_credentials_nvs();
            if (!(xEventGroupGetBits(s_wifi_event_group) & SC_STARTED)) {
                ESP_LOGI(TAG, "Creating smartconfig_task");
                xTaskCreate(smartconfig_task, "smartconfig_task", 4096, NULL, 3, NULL);
            } else {
                ESP_LOGI(TAG, "Restarting smartconfig");
                smartconfig_stop();
                smartconfig_start();
            }
        }
    } else if (event_base == IP_EVENT) {
        if (event_id == IP_EVENT_STA_GOT_IP) {
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
            ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
            s_retry_num = 0;
            xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        }
    } else if (event_base == SC_EVENT) {
        if (event_id == SC_EVENT_SCAN_DONE) {
            ESP_LOGI(TAG, "SC scan done");
        } else if (event_id == SC_EVENT_FOUND_CHANNEL) {
            ESP_LOGI(TAG, "SC found channel");
        } else if (event_id == SC_EVENT_GOT_SSID_PSWD) {
            ESP_LOGI(TAG, "SC got SSID and PSWD");
            smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
            wifi_config_t wifi_config;
            uint8_t ssid[33]     = {0};
            uint8_t password[65] = {0};
            uint8_t rvd_data[33] = {0};

            memset(&wifi_config, 0, sizeof(wifi_config_t));
            memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
            memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
            wifi_config.sta.bssid_set = evt->bssid_set;
            if (wifi_config.sta.bssid_set == true) {
                memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
            }

            memcpy(ssid, evt->ssid, sizeof(evt->ssid));
            memcpy(password, evt->password, sizeof(evt->password));
            ESP_LOGI(TAG, "SSID:%s", ssid);
            ESP_LOGI(TAG, "PASSWORD:%s", password);
            save_credentials_nvs(ssid, password);
            if (evt->type == SC_TYPE_ESPTOUCH_V2) {
                ESP_ERROR_CHECK(esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)));
                ESP_LOGI(TAG, "RVD_DATA:");
                for (int i = 0; i < 33; i++) {
                    printf("%02x ", rvd_data[i]);
                }
                printf("\n");
            }

            ESP_ERROR_CHECK(esp_wifi_disconnect());
            ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
            esp_wifi_connect();
        } else if (event_id == SC_EVENT_SEND_ACK_DONE) {
            ESP_LOGI(TAG, "SC complete");
            xEventGroupSetBits(s_wifi_event_group, SC_ESPTOUCH_DONE_BIT);
        }
    }
}