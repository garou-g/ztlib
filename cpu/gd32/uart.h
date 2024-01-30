/*******************************************************************************
 * @file    uart.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of GD32 UART driver.
 ******************************************************************************/

#pragma once

#include "periph/uart.h"
#include "gd32/gd32_types.h"
#include "etl/queue.h"

namespace gd32 {

/// @brief Queue buffer pointer for UART transmit/receive purposes
typedef etl::iqueue<uint8_t, etl::memory_model::MEMORY_MODEL_MEDIUM>* UartQueue;

/// @brief UART configuration data structure
struct UartConfig {
    int32_t uart;
    UartQueue txQueue;
    UartQueue rxQueue;
    uint32_t baudrate;
    GpioConfig tx;
    GpioConfig rx;
};

/// @brief GD32 UART peripheral driver
class Uart : public ::Uart {
public:
    Uart() = default;

    bool setConfig(const void* drvConfig) override;
    bool open() override;
    void close() override;

    bool ioctl(uint32_t cmd, void* pValue) override;

    static void irqHandler(Uart* uart);

private:
    int32_t write_(const void* buf, uint32_t len) override;
    int32_t read_(void* buf, uint32_t len) override;

    UartConfig config_;
};

}; // namespace gd32

/***************************** END OF FILE ************************************/
