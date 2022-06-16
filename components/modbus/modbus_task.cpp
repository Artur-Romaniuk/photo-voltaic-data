#include "esp_err.h"
#include "esp_log.h"
#include "esp_modbus_common.h"
#include "esp_modbus_slave.h"
#include "modbus_params.hpp"

#define TAG "Modbus"

#define MB_PORT_NUM       UART_NUM_1 // Number of UART port used for Modbus connection
#define MB_SLAVE_ADDR     (2)        // The address of device in Modbus network
#define MB_DEV_SPEED      (9600)     // The communication speed of the UART
#define UART_TXD_GPIO_NUM 19
#define UART_RXD_GPIO_NUM 18

// Defines below are used to define register start address for each type of Modbus registers
#define INPUT_OFFSET(field)      ((uint16_t)(offsetof(InputRegParams, field) >> 1))
#define MB_REG_INPUT_START_AREA0 0
#define MB_REG_INPUT_START_AREA1 (INPUT_OFFSET(forecasts[0])) // register offset input area 1
#define MB_REG_INPUT_START_AREA2 (INPUT_OFFSET(forecasts[1])) // register offset input area 2
#define MB_REG_INPUT_START_AREA3 (INPUT_OFFSET(forecasts[2])) // register offset input area 3

#define MB_READ_MASK       (MB_EVENT_INPUT_REG_RD | MB_EVENT_HOLDING_REG_RD | MB_EVENT_DISCRETE_RD | MB_EVENT_COILS_RD)
#define MB_WRITE_MASK      (MB_EVENT_HOLDING_REG_WR | MB_EVENT_COILS_WR)
#define MB_READ_WRITE_MASK (MB_READ_MASK | MB_WRITE_MASK)

#define MB_PAR_INFO_GET_TOUT (10) // Timeout for get parameter info

InputRegParams input_reg_params{};

void modbus_task(void * /*pvParameters*/) {

    void *mbc_slave_handler = nullptr;
    mbc_slave_init(MB_PORT_SERIAL_SLAVE, &mbc_slave_handler); // Initialization of Modbus controller

    // Setup communication parameters and start stack
    mb_communication_info_t comm_info;
    comm_info.mode       = MB_MODE_RTU;
    comm_info.slave_addr = MB_SLAVE_ADDR;
    comm_info.port       = MB_PORT_NUM;
    comm_info.baudrate   = MB_DEV_SPEED;
    comm_info.parity     = MB_PARITY_NONE;
    mbc_slave_setup((void *)&comm_info);

    // The code below initializes Modbus register area descriptors
    // for Modbus Holding Registers, Input Registers, Coils and Discrete Inputs
    // Initialization should be done for each supported Modbus register area according to register map.
    // When external master trying to access the register in the area that is not initialized
    // by mbc_slave_set_descriptor() API call then Modbus stack
    // will send exception response for this register area.

    // Initialization of Input Registers area
    mb_register_area_descriptor_t reg_area; // Modbus register area descriptor structure
    reg_area.type         = MB_PARAM_INPUT;
    reg_area.start_offset = 0;
    reg_area.address      = (void *)&input_reg_params.temp[0];
    reg_area.size         = 6 * sizeof(uint16_t) + kMaxNumberOfWeatherForecastsModbus * sizeof(WeatherForecast);
    ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));

    ESP_LOGI(TAG, "mbc_slave_start");
    // Starts of modbus controller and stack
    ESP_ERROR_CHECK(mbc_slave_start());

    // Set UART pin numbers
    ESP_LOGI(TAG, "uart_set_pin");
    ESP_ERROR_CHECK(uart_set_pin(MB_PORT_NUM, UART_TXD_GPIO_NUM, UART_RXD_GPIO_NUM, -1, -1));

    // Set UART driver mode to Half Duplex
    ESP_LOGI(TAG, "uart_set_mode");
    ESP_ERROR_CHECK(uart_set_mode(MB_PORT_NUM, UART_MODE_UART)); // should be RS-485 half duplex

    vTaskDelay(1000 /
               portTICK_PERIOD_MS); // initial delay, so modbus registers get filled with values from sensors, before starting communication

    mb_param_info_t reg_info; // keeps the Modbus registers access information
    while (true) {
        // Check for read/write events of Modbus master for certain events
        mb_event_group_t event = mbc_slave_check_event((mb_event_group_t)MB_READ_WRITE_MASK);

        // Filter events and process them accordingly
        if (event & MB_EVENT_INPUT_REG_RD) {
            mbc_slave_get_param_info(&reg_info, MB_PAR_INFO_GET_TOUT);
            ESP_LOGI(TAG, "INPUT READ (%u us), ADDR:%u, TYPE:%u, INST_ADDR:0x%.4x, SIZE:%u", (uint32_t)reg_info.time_stamp,
                     (uint32_t)reg_info.mb_offset, (uint32_t)reg_info.type, (uint32_t)reg_info.address, (uint32_t)reg_info.size);
        }
    }
    // Destroy of Modbus controller on alarm
    ESP_LOGI(TAG, "Modbus controller destroyed.");
    ESP_ERROR_CHECK(mbc_slave_destroy());
    vTaskDelete(nullptr);
}