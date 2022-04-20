#include "nvs_controller.hpp"

#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "wifi_init.hpp"

static const char *NVS_TAG                    = "NVS";
static const char *credentials_nvs_namespace  = "credentials";
static const char *ssid_key                   = "ssid";
static const char *password_key               = "password";


esp_err_t read_credentials_nvs(uint8_t ssid[33], uint8_t password[65]) {
    size_t ssid_size     = 33;
    size_t password_size = 65;
    nvs_handle_t nvs_handle;
    ESP_LOGI(NVS_TAG, "Opening nvs");
    esp_err_t err = nvs_open(credentials_nvs_namespace, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(NVS_TAG, "Error opening nvs: %s", esp_err_to_name(err));
        return err;
    } else {
        ESP_LOGI(NVS_TAG, "NVS openned");
        err = nvs_get_str(nvs_handle, ssid_key, (char *)ssid, &ssid_size);
        if (err != ESP_OK) {
            if (err == ESP_ERR_NVS_NOT_FOUND) {
                ESP_LOGI(NVS_TAG, "Saved SSID not found.");
            } else {
                ESP_LOGE(NVS_TAG, "NVS ERROR while reading ssid: %s", esp_err_to_name(err));
            }
            nvs_close(nvs_handle);
            return err;
        }
        err = nvs_get_str(nvs_handle, password_key, (char *)password, &password_size);
        if (err != ESP_OK) {
            if (err == ESP_ERR_NVS_NOT_FOUND) {
                ESP_LOGI(NVS_TAG, "Saved password not found.");
            } else {
                ESP_LOGE(NVS_TAG, "NVS ERROR while reading password: %s", esp_err_to_name(err));
            };
            nvs_close(nvs_handle);
            return err;
        }
        ESP_LOGI(NVS_TAG, "Read password and ssid");
        nvs_close(nvs_handle);
    }
    return ESP_OK;
}

esp_err_t save_credentials_nvs(uint8_t ssid[33], uint8_t password[65]) {
    nvs_handle_t nvs_handle;
    ESP_LOGI(NVS_TAG, "Opening nvs");
    esp_err_t err = nvs_open(credentials_nvs_namespace, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(NVS_TAG, "NVS ERROR while opening nvs: %s", esp_err_to_name(err));
        return err;
    } else {
        ESP_LOGI(NVS_TAG, "NVS openned");
        err = nvs_set_str(nvs_handle, ssid_key, (char *)ssid);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "NVS ERROR while saving ssid: %s", esp_err_to_name(err));
            nvs_close(nvs_handle);
            return err;
        }
        err = nvs_set_str(nvs_handle, password_key, (char *)password);
        if (err != ESP_OK) {
            nvs_close(nvs_handle);
            ESP_LOGE(NVS_TAG, "NVS ERROR while saving password: %s", esp_err_to_name(err));
            return err;
        }
        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "NVS ERROR while commiting: %s", esp_err_to_name(err));
        }
        ESP_LOGI(NVS_TAG, "Saved password and ssid");
        nvs_close(nvs_handle);
    }
    return ESP_OK;
}

esp_err_t erase_credentials_nvs() {
    nvs_handle_t nvs_handle;
    ESP_LOGI(NVS_TAG, "Opening nvs");
    esp_err_t err = nvs_open(credentials_nvs_namespace, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(NVS_TAG, "NVS ERROR while opening nvs: %s", esp_err_to_name(err));
    } else {
        err = nvs_erase_key(nvs_handle, ssid_key);
        err |= nvs_erase_key(nvs_handle, password_key);
    }
    ESP_LOGI(NVS_TAG, "Erased password and ssid");
    nvs_close(nvs_handle);
    return err;
}