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

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief UART peripheral driver
 */
class Uart : public PeriphDrv {
public:
    struct Config {
        int32_t uart;
        int32_t baudrate;
        int32_t tx;
        int32_t rx;
        uint32_t txBufSize;
        uint32_t rxBufSize;
    };

    enum IoctlCmd {
        kSetBaudrate,   // Sets UART baudrate
    };

    Uart();
};

#endif /* __UART_DRIVER_H */

/***************************** END OF FILE ************************************/
