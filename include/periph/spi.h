/*******************************************************************************
 * @file    spi.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of SPI bus driver.
 ******************************************************************************/

#pragma once

#include "serialdrv.h"
#include "spi_types.h"

/// @brief SPI bus peripheral driver
class Spi : public SerialDrv {
public:
    enum IoctlCmd {
        kSetCs,
        kFlushInput,
    };
};

/***************************** END OF FILE ************************************/
