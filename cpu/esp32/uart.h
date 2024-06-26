/*******************************************************************************
 * @file    uart.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of ESP32 UART driver.
 ******************************************************************************/

#pragma once

#include "periph/uart.h"

namespace esp32 {

/// @brief UART configuration data structure
struct UartConfig {
    int32_t uart;
    int32_t baudrate;
    int32_t tx;
    int32_t rx;
};

/// @brief ESP32 UART peripheral driver
class Uart : public ::Uart {
public:
    Uart() = default;

    bool setConfig(const void* drvConfig) override;
    bool open() override;
    void close() override;

    bool ioctl(uint32_t cmd, void* pValue) override;

private:
    int32_t write_(const void* buf, uint32_t len) override;
    int32_t read_(void* buf, uint32_t len) override;

    UartConfig config_;
};

}; // namespace esp32

/***************************** END OF FILE ************************************/
