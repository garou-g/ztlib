/*******************************************************************************
 * @file    uart.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   ESP32 UART driver module.
 ******************************************************************************/

#include "uart.h"

#include "driver/uart.h"
#include "driver/gpio.h"

#include <cassert>

bool Uart::open(const void* drvConfig)
{
    if (isOpen())
        return false;

    assert(drvConfig != nullptr);
    const UartConfig* config = static_cast<const UartConfig*>(drvConfig);
    if (config->uart < 0 || config->uart >= SOC_UART_NUM)
        return false;

    uart_ = config->uart;
    tx_ = config->tx;
    rx_ = config->rx;

    const uart_config_t uartConfig = {
        .baud_rate = config->baudrate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_REF_TICK,
    };
    uart_param_config(uart_, &uartConfig);
    uart_set_pin(uart_, tx_, rx_, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(uart_, 256, 256, 0, NULL, 0);
    setOpened(true);

    return true;
}

void Uart::close()
{
    if (isOpen()) {
        uart_wait_tx_done(uart_, pdMS_TO_TICKS(100));
        uart_driver_delete(uart_);
        const gpio_num_t tx = static_cast<gpio_num_t>(tx_);
        const gpio_num_t rx = static_cast<gpio_num_t>(rx_);
        gpio_reset_pin(tx);
        gpio_reset_pin(rx);
        gpio_pullup_dis(tx);
        gpio_pullup_dis(rx);
        setOpened(false);
        uart_ = -1;
        tx_ = rx_ = -1;
    }
}

int32_t Uart::write_(const void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || len == 0)
        return -1;
    return uart_write_bytes(uart_, buf, len);
}

int32_t Uart::read_(void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || len == 0)
        return -1;
    return uart_read_bytes(uart_, buf, len, 0);
}

bool Uart::ioctl(uint32_t cmd, void* pValue)
{
    if (!isOpen())
        return false;

    switch (static_cast<IoctlCmd>(cmd)) {
    case kSetBaudrate:
        if (pValue != nullptr) {
            int32_t newBaud = *static_cast<int32_t*>(pValue);
            if (newBaud > 0) {
                uart_set_baudrate(uart_, newBaud);
                return true;
            }
        }
        break;

    case kFlushInput:
        uart_flush_input(uart_);
        return true;
        break;

    case kFlushOutput:
        if (pValue != nullptr) {
            int32_t timeout = *static_cast<int32_t*>(pValue);
            uart_wait_tx_done(uart_, pdMS_TO_TICKS(timeout));
            return true;
        }
        break;

    default:
        break;
    }

    return false;
}

/***************************** END OF FILE ************************************/
