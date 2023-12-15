/*******************************************************************************
 * @file    uart.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   GD32 UART driver module.
 ******************************************************************************/

#include "uart.h"
#include "gd32_types.h"

#include <cassert>

/**
 * @brief Checks correctness of chosen uart
 *
 * @param port uart port
 * @return true if port value is correct otherwise false
 */
static bool isPortExist(uint32_t port)
{
    return port == USART0 || port == USART1
        || port == USART2
        || port == UART3 || port == UART4
#if defined(GD32F4XX_H)
        || port == UART6 || port == UART7 || port == USART5
#endif
        ;
}

/**
 * @brief Enables clock of chosen uart port
 *
 * @param port uart port
 */
static void enableClock(uint32_t port)
{
    rcu_periph_enum portClk;
    switch (port) {
    case GPIOA: portClk = RCU_GPIOA; break;
    case GPIOB: portClk = RCU_GPIOB; break;
    case GPIOC: portClk = RCU_GPIOC; break;
    case GPIOD: portClk = RCU_GPIOD; break;
    case GPIOE: portClk = RCU_GPIOE; break;
    case GPIOF: portClk = RCU_GPIOF; break;
    case GPIOG: portClk = RCU_GPIOG; break;
#if defined(GD32F4XX_H)
    case GPIOH: portClk = RCU_GPIOH; break;
    case GPIOI: portClk = RCU_GPIOI; break;
#endif
    default: return;
    }
    rcu_periph_clock_enable(portClk);
}

bool Uart::open(const void* drvConfig)
{
    if (isOpen())
        return false;

    assert(drvConfig != nullptr);
    const gd32::UartConfig* config
        = static_cast<const gd32::UartConfig*>(drvConfig);
    const UartConfig* baseConfig
        = static_cast<const UartConfig*>(config->baseConf);
    if (baseConfig->uart == 0 || !isPortExist(baseConfig->uart))
        return false;

    conf_ = *baseConfig;

    initGpioPeriph(&config->tx);
    initGpioPeriph(&config->rx);

    enableClock(conf_.uart);
    usart_deinit(conf_.uart);
    usart_baudrate_set(conf_.uart, conf_.baudrate);
    usart_receive_config(conf_.uart, USART_RECEIVE_ENABLE);
    usart_transmit_config(conf_.uart, USART_TRANSMIT_ENABLE);
    usart_enable(conf_.uart);
    setOpened(true);

    return true;
}

void Uart::close()
{
    if (isOpen()) {
        usart_disable(conf_.uart);
        usart_deinit(conf_.uart);
        setOpened(false);
        conf_.uart = 0;
    }
}

int32_t Uart::write_(const void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || len == 0)
        return -1;

    const uint8_t *data = static_cast<const uint8_t*>(buf);
    // queue.push(data[0]);
    for (uint32_t i = 0; i < len; ++i) {
        usart_data_transmit(conf_.uart, data[i]);
        while (RESET == usart_flag_get(conf_.uart, USART_FLAG_TBE));
    }
    return len;
}

int32_t Uart::read_(void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || len == 0)
        return -1;

    // TODO:
    return len;
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
                usart_baudrate_set(conf_.uart, newBaud);
                return true;
            }
        }
        break;

    case kFlushInput:
        return true;

    case kFlushOutput:
        if (pValue != nullptr) {
            while (RESET == usart_flag_get(conf_.uart, USART_FLAG_TBE));
            return true;
        }
        break;

    default:
        break;
    }

    return false;
}

/***************************** END OF FILE ************************************/
