/*******************************************************************************
 * @file    i2c.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of I2C bus driver.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2C_DRIVER_H
#define __I2C_DRIVER_H

/* Includes ------------------------------------------------------------------*/
#include "periphdrv.hpp"

/* Class definition ----------------------------------------------------------*/

/// @brief I2C bus peripheral driver
class I2c : public PeriphDrv {
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

    I2c() {}
};

#endif /* __I2C_DRIVER_H */

/***************************** END OF FILE ************************************/
