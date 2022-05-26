idf_component_register(SRCS "pressure_task.cpp" INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR} REQUIRES bmp280 error_state device i2c_config)
