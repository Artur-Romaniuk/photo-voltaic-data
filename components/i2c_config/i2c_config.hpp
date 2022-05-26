#pragma once
#include "driver/i2c.h"
#include "esp_log.h"

#define I2C_CONFIG_TAG "i2c config"
namespace i2c_config {
constexpr const int kI2cPort         = I2C_NUM_0;
constexpr const i2c_config_t kI2cCfg = {.mode          = I2C_MODE_MASTER,
                                        .sda_io_num    = GPIO_NUM_21,
                                        .scl_io_num    = GPIO_NUM_22,
                                        .sda_pullup_en = false,
                                        .scl_pullup_en = false,
                                        .master        = {.clk_speed = 100000}}; // NOLINT

enum class i2c_status { I2C_INITIALIZED, I2C_UNINITIALIZED, I2C_ERROR };

void i2c_init();

} // namespace i2c_config
