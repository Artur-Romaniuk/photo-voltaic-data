#include "i2c_config.hpp"

#include "i2cdev.h"

namespace i2c_config {

void i2c_init() {
    static i2c_status status = i2c_status::I2C_UNINITIALIZED;
    // if (status == i2c_status::I2C_ERROR) {
    //     return;
    // }

    // if (i2c_param_config(I2C_NUM_0, &kI2cCfg) != ESP_OK) {
    //     ESP_LOGE(I2C_CONFIG_TAG, "I2C config failed");
    //     status = i2c_status::I2C_ERROR;
    //     return;
    // }

    // if (i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0) == ESP_FAIL) {
    //     ESP_LOGE(I2C_CONFIG_TAG, "I2C installation failed");
    //     status = i2c_status::I2C_ERROR;
    //     return;
    // }
    i2cdev_init();
    status = i2c_status::I2C_INITIALIZED;
}
} // namespace i2c_config
