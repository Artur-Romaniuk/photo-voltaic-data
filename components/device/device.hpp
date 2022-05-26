#pragma once

#include <cstdint>
enum class device_state { DEVICE_OK, DEVICE_UNINITIALIZED, DEVICE_INIT_ERROR, DEVICE_CONNECTION_ERROR, DEVICE_ERROR };

class Device {
    device_state state_ = device_state::DEVICE_UNINITIALIZED;

        public:
    device_state get_state() { return state_; }
    void set_state(device_state state) { state_ = state; }
    virtual uint16_t get_value()    = 0;
    virtual void start_conversion() = 0;
};