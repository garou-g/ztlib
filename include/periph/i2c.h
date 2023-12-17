/*******************************************************************************
 * @file    i2c.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of I2C bus driver.
 ******************************************************************************/

#pragma once

#include "serialdrv.h"

enum class I2cMode {
    Master,
    Slave,
};

struct I2cConfig {
    int32_t i2c;
    I2cMode mode;
    uint32_t speed;
    int32_t scl;
    int32_t sda;
    bool sclPullup;
    bool sdaPullup;
};

/// @brief I2C bus peripheral driver
class I2c : public SerialDrv {
public:
    enum IoctlCmd {
        kSetAddress,    // Sets I2C device address for all next transmissions
        kSetSpeed,      // Sets I2C speed
    };

    I2c() = default;

    bool open(const void* drvConfig) override;
    void close() override;

    bool ioctl(uint32_t cmd, void* pValue) override;

private:
    int32_t write_(const void* buf, uint32_t len) override;
    int32_t read_(void* buf, uint32_t len) override;

    int32_t i2c_ = -1;
    int32_t scl_ = -1;
    int32_t sda_ = -1;
};

/***************************** END OF FILE ************************************/
