#include "esp_err.h"
#include "esp_log.h"
#include "esp_modbus_common.h"
#include "esp_modbus_slave.h"
#include "modbus_params.hpp"

#define TAG "Modbus"

#define MB_PORT_NUM           UART_NUM_1 // Number of UART port used for Modbus connection
#define MB_DEV_SPEED          (9600)     // The communication speed of the UART
#define MB_PARITY             MB_PARITY_NONE
#define MB_COMMUNICATION_MODE UART_MODE_UART // should be UART_MODE_RS485_HALF_DUPLEX for RS485
#define UART_TXD_GPIO_NUM     (19)
#define UART_RXD_GPIO_NUM     (18)

#define MB_SLAVE_ADDR        (2)  // The address of device in Modbus network
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
    comm_info.parity     = MB_PARITY;
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
    reg_area.address      = (void *)&input_reg_params;
    reg_area.size         = sizeof(input_reg_params);
    ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));

    // Starts of modbus controller and stack
    ESP_ERROR_CHECK(mbc_slave_start());

    // Set UART pin numbers
    ESP_ERROR_CHECK(uart_set_pin(MB_PORT_NUM, UART_TXD_GPIO_NUM, UART_RXD_GPIO_NUM, -1, -1));

    // Set UART driver mode to Half Duplex
    ESP_ERROR_CHECK(uart_set_mode(MB_PORT_NUM, MB_COMMUNICATION_MODE));

    vTaskDelay(1000 /
               portTICK_PERIOD_MS); // initial delay, so modbus registers get filled with values from sensors, before starting communication

    mb_param_info_t reg_info; // keeps the Modbus registers access information
    while (true) {
        mb_event_group_t event = mbc_slave_check_event((mb_event_group_t)MB_EVENT_INPUT_REG_RD);

        // Filter events and process them accordingly
        if (event & MB_EVENT_INPUT_REG_RD) {
            mbc_slave_get_param_info(&reg_info, MB_PAR_INFO_GET_TOUT);
        }
    }
    // Destroy of Modbus controller on alarm
    ESP_LOGI(TAG, "Modbus controller destroyed.");
    ESP_ERROR_CHECK(mbc_slave_destroy());
    vTaskDelete(nullptr);
}