/*******************************************************************************
 * @file    espuart.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of ESP32 UART driver.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESP_UART_DRIVER_H
#define __ESP_UART_DRIVER_H

/* Includes ------------------------------------------------------------------*/
#include "driver/uart.h"
#include "driver/gpio.h"

#include "uart.hpp"

/* Class definition ----------------------------------------------------------*/

class EspUart final : public Uart {
public:
    EspUart();

    struct Config {
        uart_port_t uart;
        gpio_num_t tx;
        gpio_num_t rx;
        int32_t baudrate;
        int32_t txBufSize;
        int32_t rxBufSize;
    };

    bool open(const void* drvConfig) override;
    void close() override;

    int32_t write(const void* buf, uint32_t len) override;
    int32_t read(void* buf, uint32_t len) override;

    bool ioctl(uint32_t cmd, void* pValue) override;

private:
    uart_port_t uart;
    gpio_num_t tx;
    gpio_num_t rx;
};

#endif /* __ESP_UART_DRIVER_H */

/***************************** END OF FILE ************************************/
