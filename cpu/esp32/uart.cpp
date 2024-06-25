/*******************************************************************************
 * @file    uart.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   ESP32 UART driver module.
 ******************************************************************************/

#include "esp32/uart.h"

#include "driver/uart.h"
#include "driver/gpio.h"

#include <cassert>

namespace esp32 {

static bool checkConfig(const esp32::UartConfig* config)
{
    assert(config != nullptr);

    return config->uart >= 0 && config->uart < SOC_UART_NUM;
}

bool Uart::setConfig(const void* drvConfig)
{
    if (isOpen())
        return false;

    const esp32::UartConfig* config = static_cast<const esp32::UartConfig*>(drvConfig);
    if (!checkConfig(config)) {
        return false;
    }
    config_ = *config;
    return true;
}

bool Uart::open()
{
    if (isOpen())
        return false;

    if (!checkConfig(&config_)) {
        return false;
    }

    const uart_config_t uartConfig = {
        .baud_rate = config_.baudrate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_REF_TICK,
    };
    uart_param_config(config_.uart, &uartConfig);
    uart_set_pin(config_.uart, config_.tx, config_.rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(config_.uart, 256, 256, 0, NULL, 0);
    setOpened(true);

    return true;
}

void Uart::close()
{
    if (isOpen()) {
        uart_wait_tx_done(config_.uart, pdMS_TO_TICKS(100));
        uart_driver_delete(config_.uart);
        const gpio_num_t tx = static_cast<gpio_num_t>(config_.tx);
        const gpio_num_t rx = static_cast<gpio_num_t>(config_.rx);
        gpio_reset_pin(tx);
        gpio_reset_pin(rx);
        gpio_pullup_dis(tx);
        gpio_pullup_dis(rx);
        setOpened(false);
    }
}

int32_t Uart::write_(const void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || len == 0)
        return -1;

    return uart_write_bytes(config_.uart, buf, len);
}

int32_t Uart::read_(void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || len == 0)
        return -1;

    return uart_read_bytes(config_.uart, buf, len, 0);
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
                uart_set_baudrate(config_.uart, newBaud);
                return true;
            }
        }
        break;

    case kFlushInput:
        uart_flush_input(config_.uart);
        return true;
        break;

    case kFlushOutput:
        if (pValue != nullptr) {
            int32_t timeout = *static_cast<int32_t*>(pValue);
            uart_wait_tx_done(config_.uart, pdMS_TO_TICKS(timeout));
            return true;
        }
        break;

    default:
        break;
    }

    return false;
}

}; // namespace esp32

/***************************** END OF FILE ************************************/
