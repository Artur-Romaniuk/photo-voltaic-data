idf_component_register(SRCS "main.cpp"
                    INCLUDE_DIRS "" REQUIRES i2c_config error_state device pressure temperature light wifi http-client esp_event esp_netif nvs_flash esp_wifi)
