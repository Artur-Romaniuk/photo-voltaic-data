idf_component_register(SRCS "thermometer_task.cpp" INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR} REQUIRES ds18x20 device modbus)
