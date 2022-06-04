idf_component_register(SRCS "http_task.cpp" "HttpClient.cpp" INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR} REQUIRES wifi modbus esp-tls json)
