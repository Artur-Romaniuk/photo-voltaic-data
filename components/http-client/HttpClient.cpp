#include "HttpClient.hpp"

#include "esp_log.h"
#include "modbus_params.hpp"
#include "web_server_config.hpp"
#include <cJSON.h>

static const char *TAG = "WEATHER_API";

constexpr size_t kServerResponseMaxSize = 2500; /**< Defines max size of a buffer, that hold server response. */

/**
 * @brief This array will be used by modbus task to send forecast data.
 *
 */
WeatherForecast forecasts[NUMBER_OF_FORECASTS];

esp_err_t HttpClient::perform() {
    esp_tls_cfg_t cfg = {.crt_bundle_attach = esp_crt_bundle_attach}; /**< Using CRT bundle for TLS encryption. */

    struct esp_tls *tls = esp_tls_conn_http_new(WEB_URL, &cfg);
    if (tls == NULL) {
        ESP_LOGE(TAG, "Could not connect to %s", WEB_URL);
        return ESP_FAIL;
    }

    char buf[kServerResponseMaxSize];
    size_t ret = 0;
    int len    = 0;

    /**
     * @brief Sending request to a server.
     *
     */
    size_t written_bytes = 0;
    do {
        ret = esp_tls_conn_write(tls, kRequest + written_bytes, sizeof(kRequest) - written_bytes);
        if (ret >= 0) {
            ESP_LOGI(TAG, "%zu bytes written", ret);
            written_bytes += ret;
        } else if (ret != ESP_TLS_ERR_SSL_WANT_READ && ret != ESP_TLS_ERR_SSL_WANT_WRITE) {
            ESP_LOGE(TAG, "esp_tls_conn_write  returned: [0x%02zX](%s)", ret, esp_err_to_name(ret));
            esp_tls_conn_delete(tls);
            return ESP_FAIL;
        }
    } while (written_bytes < sizeof(kRequest));

    ESP_LOGI(TAG, "Reading HTTP response...");

    len = sizeof(buf) - 1;
    bzero(buf, sizeof(buf));
    ret = esp_tls_conn_read(tls, (char *)buf, len);

    if (ret < 0) {
        ESP_LOGE(TAG, "esp_tls_conn_read  returned [-0x%02zX](%s)", -ret, esp_err_to_name(ret));
        return ESP_FAIL;
    }

    if (ret == 0) {
        ESP_LOGI(TAG, "connection closed");
        return ESP_OK;
    }

    len = ret;
    ESP_LOGI(TAG, "%d bytes read", len);

    /**
     * @brief Parsing server response using cJson.
     *
     */
    char *json_start = strchr(buf, '{'); /**< Finding begging and end of json inside server response by { and }. */
    char *json_end   = strrchr(buf, '}');
    if (json_start && json_end) {
        *(json_end + 1) = '\n';
        cJSON *json     = cJSON_Parse(json_start);
        cJSON *list     = cJSON_GetObjectItemCaseSensitive(json, "list");

        int i = 0;
        cJSON *forecast{};
        cJSON_ArrayForEach(forecast, list) {
            cJSON *dt         = cJSON_GetObjectItemCaseSensitive(forecast, "dt");
            cJSON *main       = cJSON_GetObjectItemCaseSensitive(forecast, "main");
            cJSON *temp       = cJSON_GetObjectItemCaseSensitive(main, "temp");
            cJSON *pressure   = cJSON_GetObjectItemCaseSensitive(main, "pressure");
            cJSON *humidity   = cJSON_GetObjectItemCaseSensitive(main, "humidity");
            cJSON *clouds     = cJSON_GetObjectItemCaseSensitive(forecast, "clouds");
            cJSON *clouds_all = cJSON_GetObjectItemCaseSensitive(clouds, "all");
            cJSON *visibility = cJSON_GetObjectItemCaseSensitive(forecast, "visibility");

            if (!cJSON_IsNumber(dt) || !cJSON_IsNumber(temp) || !cJSON_IsNumber(pressure) || !cJSON_IsNumber(humidity) ||
                !cJSON_IsNumber(clouds_all) || !cJSON_IsNumber(visibility)) {
                ESP_LOGE(TAG, "JSON parse error");
            }

            ESP_LOGI(TAG, "%d %f %d %d %d %d", dt->valueint, temp->valuedouble, pressure->valueint, humidity->valueint,
                     clouds_all->valueint, visibility->valueint);
            forecasts[i] = {.date       = static_cast<uint16_t>(dt->valueint),
                            .temp       = static_cast<uint16_t>((temp->valuedouble * 100)),
                            .clouds     = static_cast<uint16_t>(clouds_all->valueint),
                            .visibility = static_cast<uint16_t>(visibility->valueint),
                            .humidity   = static_cast<uint16_t>(humidity->valueint),
                            .pressure   = static_cast<uint16_t>(pressure->valueint)};
        }
        cJSON_free(json);
    } else {
        esp_tls_conn_delete(tls);
        return ESP_FAIL;
    }
    esp_tls_conn_delete(tls);
    return ESP_OK;
}