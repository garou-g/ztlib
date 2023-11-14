/*******************************************************************************
 * @file    espuart.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of ESP32 UART driver.
 ******************************************************************************/

#pragma once

#include "uart.hpp"

#include "driver/uart.h"
#include "driver/gpio.h"

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

    bool ioctl(uint32_t cmd, void* pValue) override;

private:
    int32_t write_(const void* buf, uint32_t len) override;
    int32_t read_(void* buf, uint32_t len) override;

    uart_port_t uart;
    gpio_num_t tx;
    gpio_num_t rx;
};

/***************************** END OF FILE ************************************/
