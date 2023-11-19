/*******************************************************************************
 * @file    i2c.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   ESP32 I2C bus driver.
 ******************************************************************************/

#include "i2c.h"

#include "driver/i2c.h"

#include <cassert>

/// I2C driver operation timeout in milliseconds
static constexpr uint32_t timeoutMs = pdMS_TO_TICKS(100);

bool I2c::open(const void* drvConfig)
{
    if (isOpen())
        return false;

    assert(drvConfig != nullptr);
    const I2cConfig* config = static_cast<const I2cConfig*>(drvConfig);
    if (config->i2c < 0 || config->i2c >= SOC_I2C_NUM)
        return false;

    i2c_ = config->i2c;
    scl_ = config->scl;
    sda_ = config->sda;

    const i2c_config_t i2cConfig = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_,
        .scl_io_num = scl_,
        .sda_pullup_en = config->sdaPullup ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .scl_pullup_en = config->sclPullup ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .master = { config->speed },
        .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL,
    };
    i2c_param_config(i2c_, &i2cConfig);
    i2c_set_timeout(i2c_, 0xFFFFF);
    i2c_driver_install(i2c_, I2C_MODE_MASTER, 0, 0, 0);
    setOpened(true);

    return true;
}

void I2c::close()
{
    if (isOpen()) {
        i2c_driver_delete(i2c_);
        gpio_reset_pin(static_cast<gpio_num_t>(scl_));
        gpio_reset_pin(static_cast<gpio_num_t>(sda_));
        gpio_pullup_dis(static_cast<gpio_num_t>(scl_));
        gpio_pullup_dis(static_cast<gpio_num_t>(sda_));
        setOpened(false);
        i2c_ = -1;
        scl_ = sda_ = -1;
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
    esp_err_t ret = i2c_master_cmd_begin(i2c_, cmd, timeoutMs);
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
    esp_err_t ret = i2c_master_cmd_begin(i2c_, cmd, timeoutMs);
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
            setAddr(*static_cast<int32_t*>(pValue) << 1);
            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

/***************************** END OF FILE ************************************/
