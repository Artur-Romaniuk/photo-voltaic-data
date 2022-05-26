idf_component_register(SRCS "thermometer_task.cpp" INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR} REQUIRES esp32-ds18b20 esp32-owb error_state)
