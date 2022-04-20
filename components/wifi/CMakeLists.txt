idf_component_register(SRCS "events.cpp" "nvs_controller.cpp" "wifi_init.cpp" "smartconfig.cpp" INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}
REQUIRES esp_event esp_netif nvs_flash esp_wifi)
