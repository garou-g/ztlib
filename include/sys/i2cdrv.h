/*******************************************************************************
 * @file    i2cdrv.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Abstract I2C driver.
 ******************************************************************************/

#pragma once

#include "periph/i2c.h"

/**
 * @brief Base I2C module driver class
 */
class I2cDrv {
public:
    /**
     * @brief Construct a new I2cDrv object
     *
     * @param drv I2C driver instance
     */
    I2cDrv(I2c& drv)
        : i2c(&drv)
    {
    }

protected:
    /// @brief I2C driver instance for successors
    I2c* const i2c;
};

/***************************** END OF FILE ************************************/
