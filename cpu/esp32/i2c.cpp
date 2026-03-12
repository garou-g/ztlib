/*******************************************************************************
 * @file    i2c.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   ESP32 I2C bus driver.
 ******************************************************************************/

#pragma clang diagnostic ignored "-Wvla-cxx-extension"

#include "esp32/i2c.h"
#include "esp_err.h"
#include <cstdint>
#include <string.h>

#if (ESP_IDF_VERSION_MAJOR == 5)
#include "driver/i2c_master.h"
#else
#include "driver/i2c.h"
#endif

#include <cassert>

namespace esp32 {

/// I2C driver operation timeout in milliseconds
static constexpr uint32_t timeoutMs = pdMS_TO_TICKS(100);

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

#if (ESP_IDF_VERSION_MAJOR == 5)
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
#else
    const i2c_config_t i2cConfig = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = config_.sda,
        .scl_io_num = config_.scl,
        .sda_pullup_en = config_.sdaPullup ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .scl_pullup_en = config_.sclPullup ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .master = { static_cast<uint32_t>(config_.speed) },
        .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL,
    };
    i2c_param_config(config_.i2c, &i2cConfig);
    i2c_set_timeout(config_.i2c, 0xFFFFF);
    i2c_driver_install(config_.i2c, I2C_MODE_MASTER, 0, 0, 0);
#endif
    setOpened(true);

    return true;
}

void I2c::close()
{
    if (isOpen()) {
#if (ESP_IDF_VERSION_MAJOR == 5)
        i2c_master_bus_rm_device(dev_);
        i2c_del_master_bus(bus_);
#else
        i2c_driver_delete(config_.i2c);
        const gpio_num_t scl = static_cast<gpio_num_t>(config_.scl);
        const gpio_num_t sda = static_cast<gpio_num_t>(config_.sda);
        gpio_reset_pin(scl);
        gpio_reset_pin(sda);
        gpio_pullup_dis(scl);
        gpio_pullup_dis(sda);
#endif
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

#if (ESP_IDF_VERSION_MAJOR == 5)
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
#else
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr | I2C_MASTER_WRITE, true);
    // Without register just read data as is
    if (getReg() >= 0)
        i2c_master_write_byte(cmd, getReg(), true);
    // When data length is zero make exchange without data like destination test
    if (len != 0)
        i2c_master_write(cmd, bytes, len, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(config_.i2c, cmd, timeoutMs);
    i2c_cmd_link_delete(cmd);
#endif
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

#if (ESP_IDF_VERSION_MAJOR == 5)
    i2c_master_device_change_address(dev_, getAddr(), timeoutMs);

    const int32_t reg = getReg();
    esp_err_t ret;
    if (reg >= 0) {
        ret = i2c_master_transmit_receive(dev_, (const uint8_t*)&reg, 1,
            bytes, len, timeoutMs);
    } else {
        ret = i2c_master_receive(dev_, bytes, len, timeoutMs);
    }
#else
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    // Without register just read data as is
    if (getReg() >= 0) {
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, addr | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, getReg(), true);
    }
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr | I2C_MASTER_READ, true);
    if (len == 1) {
        i2c_master_read_byte(cmd, bytes, I2C_MASTER_NACK);
    } else {
        i2c_master_read(cmd, bytes, len - 1, I2C_MASTER_ACK);
        i2c_master_read_byte(cmd, &bytes[len - 1], I2C_MASTER_NACK);
    }
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(config_.i2c, cmd, timeoutMs);
    i2c_cmd_link_delete(cmd);
#endif
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
