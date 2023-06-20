/*******************************************************************************
 * @file    gd32uart.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of GD32 UART driver.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GD32_UART_DRIVER_H
#define __GD32_UART_DRIVER_H

/* Includes ------------------------------------------------------------------*/
#include "gd32f30x.h"

#include "uart.hpp"
#include "gd32gpio.hpp"

/* Class definition ----------------------------------------------------------*/

class Gd32Uart final : public Uart {
public:
    Gd32Uart();

    struct Config {
        uint32_t uart;
        Gd32Gpio::Config tx;
        Gd32Gpio::Config rx;
        int32_t baudrate;
    };

    bool open(const void* drvConfig) override;
    void close() override;

    int32_t write(const void* buf, uint32_t len) override;
    int32_t read(void* buf, uint32_t len) override;

    bool ioctl(uint32_t cmd, void* pValue) override;

private:
    uint32_t uart;
};

#endif /* __GD32_UART_DRIVER_H */

/***************************** END OF FILE ************************************/
