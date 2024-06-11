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

/// @brief UART configuration data structure
struct UartConfig {
    int32_t uart;
    uint32_t baudrate;
    GpioConfig tx;
    GpioConfig rx;
};

/// @brief Private realization of GD32 UART peripheral driver
class P_Uart : public ::Uart {
public:
    using IUartQueue = etl::iqueue<uint8_t, etl::memory_model::MEMORY_MODEL_MEDIUM>;

    P_Uart(IUartQueue& tx, IUartQueue& rx);

    bool setConfig(const void* drvConfig) override;
    bool open() override;
    void close() override;

    bool ioctl(uint32_t cmd, void* pValue) override;

    static void irqHandler(P_Uart* uart);

private:
    int32_t write_(const void* buf, uint32_t len) override;
    int32_t read_(void* buf, uint32_t len) override;

    UartConfig config_;
    IUartQueue& txQueue_;
    IUartQueue& rxQueue_;
};

template <size_t SIZE>
class Uart final : public P_Uart {
public:
    using UartQueue = etl::queue<uint8_t, SIZE, etl::memory_model::MEMORY_MODEL_MEDIUM>;

    Uart()
        : P_Uart(txQueue_, rxQueue_)
    {
    }

private:
    UartQueue txQueue_;
    UartQueue rxQueue_;
};

}; // namespace gd32

/***************************** END OF FILE ************************************/
