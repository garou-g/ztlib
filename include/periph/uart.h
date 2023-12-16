/*******************************************************************************
 * @file    uart.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of UART driver.
 ******************************************************************************/

#pragma once

#include "serialdrv.h"
#include "etl/queue.h"

/// @brief Queue buffer pointer for UART transmit/receive purposes
typedef etl::iqueue<uint8_t, etl::memory_model::MEMORY_MODEL_SMALL>* UartQueue;

/// @brief UART configuration data structure
struct UartConfig {
    int32_t uart;
    UartQueue txQueue;
    UartQueue rxQueue;
    uint32_t baudrate;
};

/// @brief UART peripheral driver
class Uart : public SerialDrv {
public:
    enum IoctlCmd {
        kSetBaudrate,   // Sets UART baudrate
        kFlushInput,
        kFlushOutput,   // Blocks until output will be empty or timeout comes
    };

    Uart() = default;

    bool open(const void* drvConfig) override;
    void close() override;

    bool ioctl(uint32_t cmd, void* pValue) override;

    static void irqHandler(Uart* uart);

private:
    int32_t write_(const void* buf, uint32_t len) override;
    int32_t read_(void* buf, uint32_t len) override;

    UartConfig conf_ = {
        .uart = -1,
        .txQueue = nullptr,
        .rxQueue = nullptr,
        .baudrate = 0,
    };
};

/***************************** END OF FILE ************************************/
