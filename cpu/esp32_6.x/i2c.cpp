/*******************************************************************************
 * @file    i2c.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   ESP32 I2C bus driver.
 ******************************************************************************/

#include "esp32_6.x/i2c.h"
#include "esp_err.h"
#include <cstdint>
#include <string.h>

#include "driver/i2c_master.h"

#include <cassert>

namespace esp32 {

/// I2C driver operation timeout in milliseconds
static constexpr uint32_t timeoutMs = pdMS_TO_TICKS(500);

static bool checkConfig(const esp32::I2cConfig* config)
{
    assert(config != nullptr);

    return config->i2c >= 0 && config->i2c < SOC_I2C_NUM;
}

bool I2c::setConfig(const void* drvConfig)
{
    if (isOpen())
        return false;

    const esp32::I2cConfig* config = static_cast<const esp32::I2cConfig*>(drvConfig);
    if (!checkConfig(config)) {
        return false;
    }
    config_ = *config;
    return true;
}

bool I2c::open()
{
    if (isOpen())
        return false;

    if (!checkConfig(&config_)) {
        return false;
    }

    const i2c_master_bus_config_t busConfig = {
        .i2c_port = config_.i2c,
        .sda_io_num = static_cast<gpio_num_t>(config_.sda),
        .scl_io_num = static_cast<gpio_num_t>(config_.scl),
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        .trans_queue_depth = 0,
        .flags = {
            .enable_internal_pullup = (config_.sdaPullup || config_.sclPullup) ? 1U : 0U,
            .allow_pd = 0U,
        },
    };
    esp_err_t ret = i2c_new_master_bus(&busConfig, &bus_);
    if (ret != ESP_OK) {
        return false;
    }

    const i2c_device_config_t devConfig = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0,
        .scl_speed_hz = static_cast<uint32_t>(config_.speed),
        .scl_wait_us = 0,
        .flags = { .disable_ack_check = 0 },
    };
    ret = i2c_master_bus_add_device(bus_, &devConfig, &dev_);
    if (ret != ESP_OK) {
        i2c_del_master_bus(bus_);
        return false;
    }
    setOpened(true);

    return true;
}

void I2c::close()
{
    if (isOpen()) {
        i2c_master_bus_rm_device(dev_);
        i2c_del_master_bus(bus_);
        setOpened(false);
    }
}

int32_t I2c::write_(const void* buf, uint32_t len)
{
    assert(buf != nullptr);

    // Take address from parent first if exists
    int32_t addr = getAddr() >= 0 ? (getAddr() << 1) : -1;
    if (!isOpen() || addr < 0)
        return -1;

    const uint8_t* bytes = static_cast<const uint8_t*>(buf);

    i2c_master_device_change_address(dev_, getAddr(), timeoutMs);
    const int32_t reg = getReg();
    esp_err_t ret;
    if (reg >= 0) {
        uint8_t bytesWithReg[len + 1];
        bytesWithReg[0] = reg;
        memcpy(&bytesWithReg[1], bytes, len);
        ret = i2c_master_transmit(dev_, bytesWithReg, len + 1, timeoutMs);
    } else {
        ret = i2c_master_transmit(dev_, bytes, len, timeoutMs);
    }
    return ret == ESP_OK ? len : -1;
}

int32_t I2c::read_(void* buf, uint32_t len)
{
    assert(buf != nullptr);

    // Take address from parent first if exists
    int32_t addr = getAddr() >= 0 ? (getAddr() << 1) : -1;
    if (!isOpen() || addr < 0 || len == 0)
        return -1;

    uint8_t* bytes = static_cast<uint8_t*>(buf);

    i2c_master_device_change_address(dev_, getAddr(), timeoutMs);

    const int32_t reg = getReg();
    esp_err_t ret;
    if (reg >= 0) {
        ret = i2c_master_transmit_receive(dev_, (const uint8_t*)&reg, 1,
            bytes, len, timeoutMs);
    } else {
        ret = i2c_master_receive(dev_, bytes, len, timeoutMs);
    }
    return ret == ESP_OK ? len : -1;
}

bool I2c::ioctl(uint32_t cmd, void* pValue)
{
    if (!isOpen())
        return false;

    switch (cmd) {
    case kSetAddress:
        if (pValue != nullptr) {
            setAddr(*static_cast<int32_t*>(pValue));
            return true;
        }
        break;
    case kSetSpeed:
        if (pValue != nullptr) {
            return true;
        }
    default:
        break;
    }

    return false;
}

}; // namespace esp32

/***************************** END OF FILE ************************************/
