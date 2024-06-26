/*******************************************************************************
 * @file    i2c.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   ESP32 I2C bus driver.
 ******************************************************************************/

#include "esp32/i2c.h"

#include "driver/i2c.h"

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
    setOpened(true);

    return true;
}

void I2c::close()
{
    if (isOpen()) {
        i2c_driver_delete(config_.i2c);
        const gpio_num_t scl = static_cast<gpio_num_t>(config_.scl);
        const gpio_num_t sda = static_cast<gpio_num_t>(config_.sda);
        gpio_reset_pin(scl);
        gpio_reset_pin(sda);
        gpio_pullup_dis(scl);
        gpio_pullup_dis(sda);
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

    return ret == ESP_OK ? len : -1;
}

bool I2c::ioctl(uint32_t cmd, void* pValue)
{
    if (!isOpen())
        return false;

    switch (static_cast<IoctlCmd>(cmd)) {
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
