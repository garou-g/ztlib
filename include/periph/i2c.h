/*******************************************************************************
 * @file    i2c.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of I2C bus driver.
 ******************************************************************************/

#pragma once

#include "serialdrv.h"

/// @brief I2C bus peripheral driver
class I2c : public SerialDrv {
public:
    enum IoctlCmd {
        kSetAddress,    // Sets I2C device address for all next transmissions
    };

    enum Mode {
        kMaster,
        kSlave,
    };

    struct Config {
        int32_t i2c;
        Mode mode;
        uint32_t speed;
        int32_t scl;
        int32_t sda;
        bool sclPullup;
        bool sdaPullup;
    };

    I2c() = default;
};

/***************************** END OF FILE ************************************/
