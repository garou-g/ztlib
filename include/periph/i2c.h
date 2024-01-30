/*******************************************************************************
 * @file    i2c.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of I2C bus driver.
 ******************************************************************************/

#pragma once

#include "serialdrv.h"
#include "i2c_types.h"

/// @brief I2C bus peripheral driver
class I2c : public SerialDrv {
public:
    enum IoctlCmd {
        kSetAddress,    // Sets I2C device address for all next transmissions
        kSetSpeed,      // Sets I2C speed
    };
};

/***************************** END OF FILE ************************************/
