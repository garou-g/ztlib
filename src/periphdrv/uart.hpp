/*******************************************************************************
 * @file    uart.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of UART driver.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_DRIVER_H
#define __UART_DRIVER_H

/* Includes ------------------------------------------------------------------*/
#include "periphdrv.hpp"

/* Class definition ----------------------------------------------------------*/

/// @brief UART peripheral driver
class Uart : public PeriphDrv {
public:
    enum IoctlCmd {
        kSetBaudrate,   // Sets UART baudrate
        kFlushInput,
        kFlushOutput,   // Blocks until output will be empty or timeout comes
    };

    Uart() {}
};

#endif /* __UART_DRIVER_H */

/***************************** END OF FILE ************************************/
