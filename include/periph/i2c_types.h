/*******************************************************************************
 * @file    i2c_types.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of common I2C types.
 ******************************************************************************/

#pragma once

enum class I2cMode {
    Master,
    Slave,
};

enum class I2cSpeed {
    Speed100 = 100000,
    Speed400 = 400000,
};

/***************************** END OF FILE ************************************/
