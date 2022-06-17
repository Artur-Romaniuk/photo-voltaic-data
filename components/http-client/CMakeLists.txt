idf_component_register(SRCS "web_server_config.cpp" "http_task.cpp" "HttpClient.cpp" INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR} REQUIRES wifi modbus esp-tls json)
