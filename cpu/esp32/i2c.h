/*******************************************************************************
 * @file    i2c.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of ESP32 I2C bus driver.
 ******************************************************************************/

#pragma once

#include "periph/i2c.h"

namespace esp32 {

struct I2cConfig {
    int32_t i2c;
    I2cMode mode;
    I2cSpeed speed;
    int32_t scl;
    int32_t sda;
    bool sclPullup;
    bool sdaPullup;
};

/// @brief ESP32 I2C bus peripheral driver
class I2c : public ::I2c {
public:
    I2c() = default;

    bool setConfig(const void* drvConfig) override;
    bool open() override;
    void close() override;

    bool ioctl(uint32_t cmd, void* pValue) override;

private:
    int32_t write_(const void* buf, uint32_t len) override;
    int32_t read_(void* buf, uint32_t len) override;

    I2cConfig config_;
};

}; // namespace esp32

/***************************** END OF FILE ************************************/
