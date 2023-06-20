/*******************************************************************************
 * @file    gd32uart.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   GD32 UART driver module.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "gd32uart.hpp"

#include <cassert>

/**
 * @brief Default empty constructor
 */
Gd32Uart::Gd32Uart()
    : Uart()
    , uart(0)
{
}

bool Gd32Uart::open(const void* drvConfig)
{
    if (isOpen())
        return false;

    assert(drvConfig != nullptr);
    const Gd32Uart::Config* config
        = static_cast<const Gd32Uart::Config*>(drvConfig);

    if (config->uart == 0 || !(config->uart == USART0
        || config->uart == USART1 || config->uart == USART2
        || config->uart == UART3 || config->uart == UART4))
        return false;
    uart = config->uart;

    rcu_periph_enum uartClock;
    switch (uart) {
    default:
    case USART0: uartClock = RCU_USART0; break;
    case USART1: uartClock = RCU_USART1; break;
    case USART2: uartClock = RCU_USART2; break;
    case UART3: uartClock = RCU_UART3; break;
    case UART4: uartClock = RCU_UART4; break;
    }
    rcu_periph_clock_enable(uartClock);
    rcu_periph_clock_enable(config->tx.clock);
    rcu_periph_clock_enable(config->rx.clock);

    gpio_init(config->tx.port, config->tx.mode, GPIO_OSPEED_50MHZ, config->tx.pin);
    gpio_init(config->rx.port, config->rx.mode, GPIO_OSPEED_50MHZ, config->rx.pin);

    usart_deinit(uart);
    usart_baudrate_set(uart, config->baudrate);
    usart_receive_config(uart, USART_RECEIVE_ENABLE);
    usart_transmit_config(uart, USART_TRANSMIT_ENABLE);
    usart_enable(uart);
    setOpened(true);

    return true;
}

void Gd32Uart::close()
{
    if (isOpen()) {
        usart_disable(uart);
        usart_deinit(uart);
        setOpened(false);
        uart = 0;
    }
}

int32_t Gd32Uart::write(const void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || len == 0)
        return -1;

    const uint8_t *data = static_cast<const uint8_t*>(buf);
    for (uint32_t i = 0; i < len; ++i) {
        usart_data_transmit(uart, data[i]);
        while (RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    }
    return len;
}

int32_t Gd32Uart::read(void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || len == 0)
        return -1;

    // TODO:
    return len;
}

bool Gd32Uart::ioctl(uint32_t cmd, void* pValue)
{
    if (!isOpen())
        return false;

    switch (static_cast<IoctlCmd>(cmd))
    {
    case kSetBaudrate:
        if (pValue != nullptr) {
            int32_t newBaud = *static_cast<int32_t*>(pValue);
            if (newBaud > 0) {
                usart_baudrate_set(uart, newBaud);
                return true;
            }
        }
        break;

    case kFlushInput:
        return true;

    case kFlushOutput:
        if (pValue != nullptr) {
            while (RESET == usart_flag_get(uart, USART_FLAG_TBE));
            return true;
        }
        break;

    default:
        break;
    }

    return false;
}

/***************************** END OF FILE ************************************/
