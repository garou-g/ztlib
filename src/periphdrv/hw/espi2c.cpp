/*******************************************************************************
 * @file    espi2c.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   ESP32 I2C bus driver.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <cassert>

#include "driver/i2c.h"

#include "espi2c.hpp"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/// I2C driver operation timeout in milliseconds
const uint32_t EspI2c::timeoutMs = pdMS_TO_TICKS(100);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Default empty constructor
 */
EspI2c::EspI2c()
    : I2c()
    , i2c(-1)
    , addr(-1)
{
}

bool EspI2c::open(const void* const drvConfig)
{
    if (isOpen())
        return false;

    assert(drvConfig != nullptr);
    const I2c::Config* const config
        = static_cast<const I2c::Config*>(drvConfig);

    if (config->i2c < 0 || config->i2c >= SOC_I2C_NUM)
        return false;
    i2c = config->i2c;

    const i2c_config_t i2cConfig = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = config->sda,
        .scl_io_num = config->scl,
        .sda_pullup_en = config->sdaPullup ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .scl_pullup_en = config->sclPullup ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .master = { config->speed },
        .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL,
    };
    i2c_param_config(i2c, &i2cConfig);
    i2c_set_timeout(i2c, 0xFFFFF);
    i2c_driver_install(i2c, I2C_MODE_MASTER, 0, 0, 0);
    setOpened(true);

    return true;
}

void EspI2c::close()
{
    if (isOpen()) {
        i2c_driver_delete(i2c);
        setOpened(false);
        i2c = -1;
        addr = -1;
    }
}

int32_t EspI2c::write(const void* const buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || addr < 0)
        return -1;

    const uint8_t* const bytes = static_cast<const uint8_t*>(buf);
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
    esp_err_t ret = i2c_master_cmd_begin(i2c, cmd, timeoutMs);
    i2c_cmd_link_delete(cmd);

    return ret == ESP_OK;
}

int32_t EspI2c::read(void* const buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || addr < 0 || len == 0)
        return -1;

    uint8_t* const bytes = static_cast<uint8_t*>(buf);
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
    esp_err_t ret = i2c_master_cmd_begin(i2c, cmd, timeoutMs);
    i2c_cmd_link_delete(cmd);

    return ret == ESP_OK;
}

bool EspI2c::ioctl(uint32_t cmd, void* const pValue)
{
    if (!isOpen())
        return false;

    switch (static_cast<IoctlCmd>(cmd))
    {
    case kSetAddress:
        if (pValue != nullptr) {
            addr = *static_cast<int32_t*>(pValue) << 1;
            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

/* Private functions ---------------------------------------------------------*/

/***************************** END OF FILE ************************************/
