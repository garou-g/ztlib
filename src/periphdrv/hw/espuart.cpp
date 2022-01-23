/*******************************************************************************
 * @file    espuart.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   ESP32 UART driver module.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <cassert>

#include "driver/uart.h"

#include "espuart.hpp"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief Default empty constructor
 */
EspUart::EspUart()
    : Uart()
    , uart(-1)
{
}

bool EspUart::open(const void* const drvConfig)
{
    if (isOpen())
        return false;

    assert(drvConfig != nullptr);
    const Uart::Config* const config
        = static_cast<const Uart::Config*>(drvConfig);

    if (config->uart < 0 || config->uart >= SOC_UART_NUM)
        return false;
    uart = config->uart;

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
    uart_set_pin(uart, config->tx, config->rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(uart, config->rxBufSize, config->txBufSize, 0, NULL, 0);
    setOpened(true);

    return true;
}

void EspUart::close()
{
    if (isOpen()) {
        uart_driver_delete(uart);
        setOpened(false);
        uart = -1;
    }
}

int32_t EspUart::write(const void* const buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || len == 0)
        return -1;

    return uart_write_bytes(uart, buf, len);
}

int32_t EspUart::read(void* const buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || len == 0)
        return -1;

    return uart_read_bytes(uart, buf, len, 0);
}

bool EspUart::ioctl(uint32_t cmd, void* const pValue)
{
    if (!isOpen())
        return false;

    switch (static_cast<IoctlCmd>(cmd))
    {
    case kSetBaudrate:
        if (pValue != nullptr) {
            int32_t newBaud = *static_cast<int32_t*>(pValue) << 1;
            if (newBaud > 0) {
                uart_set_baudrate(uart, newBaud);
                return true;
            }
        }
        break;
    default:
        break;
    }

    return false;
}

/* Private functions ---------------------------------------------------------*/

/***************************** END OF FILE ************************************/