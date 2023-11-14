/*******************************************************************************
 * @file    espuart.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   ESP32 UART driver module.
 ******************************************************************************/

#include "espuart.hpp"

#include <cassert>

/**
 * @brief Default empty constructor
 */
EspUart::EspUart()
    : Uart()
    , uart(-1)
    , tx(GPIO_NUM_NC)
    , rx(GPIO_NUM_NC)
{
}

bool EspUart::open(const void* drvConfig)
{
    if (isOpen())
        return false;

    assert(drvConfig != nullptr);
    const EspUart::Config* config
        = static_cast<const EspUart::Config*>(drvConfig);

    if (config->uart < 0 || config->uart >= SOC_UART_NUM)
        return false;
    uart = config->uart;
    tx = config->tx;
    rx = config->rx;

    const uart_config_t uartConfig = {
        .baud_rate = config->baudrate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_REF_TICK,
    };
    uart_param_config(uart, &uartConfig);
    uart_set_pin(uart, tx, rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(uart, config->rxBufSize, config->txBufSize, 0, NULL, 0);
    setOpened(true);

    return true;
}

void EspUart::close()
{
    if (isOpen()) {
        uart_wait_tx_done(uart, pdMS_TO_TICKS(100));
        uart_driver_delete(uart);
        gpio_reset_pin(tx);
        gpio_reset_pin(rx);
        gpio_pullup_dis(tx);
        gpio_pullup_dis(rx);
        setOpened(false);
        uart = -1;
        tx = rx = GPIO_NUM_NC;
    }
}

int32_t EspUart::write_(const void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || len == 0)
        return -1;

    return uart_write_bytes(uart, buf, len);
}

int32_t EspUart::read_(void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || len == 0)
        return -1;

    return uart_read_bytes(uart, buf, len, 0);
}

bool EspUart::ioctl(uint32_t cmd, void* pValue)
{
    if (!isOpen())
        return false;

    switch (static_cast<IoctlCmd>(cmd))
    {
    case kSetBaudrate:
        if (pValue != nullptr) {
            int32_t newBaud = *static_cast<int32_t*>(pValue);
            if (newBaud > 0) {
                uart_set_baudrate(uart, newBaud);
                return true;
            }
        }
        break;

    case kFlushInput:
        uart_flush_input(uart);
        return true;
        break;

    case kFlushOutput:
        if (pValue != nullptr) {
            int32_t timeout = *static_cast<int32_t*>(pValue);
            uart_wait_tx_done(uart, pdMS_TO_TICKS(timeout));
            return true;
        }
        break;

    default:
        break;
    }

    return false;
}

/***************************** END OF FILE ************************************/
