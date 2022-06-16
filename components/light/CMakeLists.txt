idf_component_register(SRCS "light_task.cpp" INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR} REQUIRES bh1750 device i2c_config modbus)
