idf_component_register(SRCS "modbus_task.cpp" INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR} REQUIRES freemodbus http-client light pressure_humidity temperature)
