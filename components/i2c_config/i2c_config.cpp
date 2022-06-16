#include "i2c_config.hpp"

#include "i2cdev.h"

namespace i2c_config {

void i2c_init() {
    static i2c_status status = i2c_status::I2C_UNINITIALIZED;
    if (status == i2c_status::I2C_UNINITIALIZED) {
        i2cdev_init();
        status = i2c_status::I2C_INITIALIZED;
    }
}
} // namespace i2c_config
