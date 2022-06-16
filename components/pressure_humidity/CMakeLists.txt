idf_component_register(SRCS "pressure_humidity_task.cpp" INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR} REQUIRES bmp280 dht device i2c_config modbus)
