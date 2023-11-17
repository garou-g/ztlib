/*******************************************************************************
 * @file    gd32uart.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of GD32 UART driver.
 ******************************************************************************/

#pragma once

#include "uart.hpp"
#include "gd32gpio.hpp"
#include "gd32.h"

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

    bool ioctl(uint32_t cmd, void* pValue) override;

private:
    int32_t write_(const void* buf, uint32_t len) override;
    int32_t read_(void* buf, uint32_t len) override;

    uint32_t uart;
};

/***************************** END OF FILE ************************************/
