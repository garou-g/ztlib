/*******************************************************************************
 * @file    uart.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of UART driver.
 ******************************************************************************/

#pragma once

#include "serialdrv.h"

struct UartConfig {
    int32_t uart;
    int32_t tx;
    int32_t rx;
    int32_t baudrate;
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

private:
    int32_t write_(const void* buf, uint32_t len) override;
    int32_t read_(void* buf, uint32_t len) override;

    int32_t uart_;
    int32_t tx_;
    int32_t rx_;
};

/***************************** END OF FILE ************************************/
