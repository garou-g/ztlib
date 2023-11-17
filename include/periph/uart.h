/*******************************************************************************
 * @file    uart.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of UART driver.
 ******************************************************************************/

#pragma once

#include "serialdrv.h"

/// @brief UART peripheral driver
class Uart : public SerialDrv {
public:
    enum IoctlCmd {
        kSetBaudrate,   // Sets UART baudrate
        kFlushInput,
        kFlushOutput,   // Blocks until output will be empty or timeout comes
    };

    Uart() = default;
};

/***************************** END OF FILE ************************************/
