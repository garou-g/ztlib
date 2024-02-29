/*******************************************************************************
 * @file    uart.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   GD32 UART driver module.
 ******************************************************************************/

#include "gd32/uart.h"
#include "gd32/gd32_types.h"

#include <cassert>

namespace gd32 {

static Uart* uartInstances[8] = { nullptr };

static bool checkConfig(const gd32::UartConfig* config)
{
    assert(config != nullptr);

    return config->txQueue != nullptr && config->rxQueue != nullptr
        && (config->uart == USART0 || config->uart == USART1
        || config->uart == USART2
        || config->uart == UART3 || config->uart == UART4
#if defined(GD32F4XX_H)
        || config->uart == USART5
        || config->uart == UART6 || config->uart == UART7
#endif
        );
}

static void setHandler(uint32_t port, Uart* arg)
{
    switch (port) {
    case USART0: uartInstances[0] = arg; break;
    case USART1: uartInstances[1] = arg; break;
    case USART2: uartInstances[2] = arg; break;
    case UART3: uartInstances[3] = arg; break;
    case UART4: uartInstances[4] = arg; break;
#if defined(GD32F4XX_H)
    case USART5: uartInstances[5] = arg; break;
    case UART6: uartInstances[6] = arg; break;
    case UART7: uartInstances[7] = arg; break;
#endif
    default: return;
    }
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
    case USART0: portClk = RCU_USART0; break;
    case USART1: portClk = RCU_USART1; break;
    case USART2: portClk = RCU_USART2; break;
    case UART3: portClk = RCU_UART3; break;
    case UART4: portClk = RCU_UART4; break;
#if defined(GD32F4XX_H)
    case USART5: portClk = RCU_USART5; break;
    case UART6: portClk = RCU_UART6; break;
    case UART7: portClk = RCU_UART7; break;
#endif
    default: return;
    }
    rcu_periph_clock_enable(portClk);
}

static void setIrq(uint32_t port, bool enable)
{
    IRQn_Type irqType;
    switch (port) {
    case USART0: irqType = USART0_IRQn; break;
    case USART1: irqType = USART1_IRQn; break;
    case USART2: irqType = USART2_IRQn; break;
#if defined(GD32F4XX_H) || defined(GD32F10X_HD)
    case UART3: irqType = UART3_IRQn; break;
    case UART4: irqType = UART4_IRQn; break;
#endif
#if defined(GD32F407) || defined(GD32F427) || \
    defined (GD32F405) || defined (GD32F425) || \
    defined (GD32F450) || defined (GD32F470)
    case USART5: irqType = USART5_IRQn; break;
#endif
#if defined (GD32F450) || defined (GD32F470)
    case UART6: irqType = UART6_IRQn; break;
    case UART7: irqType = UART7_IRQn; break;
#endif
    default: return;
    }

    if (enable) {
        nvic_irq_enable(irqType, 0, 0);
    } else {
        nvic_irq_disable(irqType);
    }
}

bool Uart::setConfig(const void* drvConfig)
{
    if (isOpen())
        return false;

    const gd32::UartConfig* config = static_cast<const gd32::UartConfig*>(drvConfig);
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

    initGpioPeriph(&config_.tx);
    initGpioPeriph(&config_.rx);

    enableClock(config_.uart);
    usart_deinit(config_.uart);
    usart_baudrate_set(config_.uart, config_.baudrate);
    usart_receive_config(config_.uart, USART_RECEIVE_ENABLE);
    usart_transmit_config(config_.uart, USART_TRANSMIT_ENABLE);
    usart_enable(config_.uart);
    usart_interrupt_enable(config_.uart, USART_INT_RBNE);
    setHandler(config_.uart, this);
    setIrq(config_.uart, true);
    setOpened(true);

    return true;
}

void Uart::close()
{
    if (isOpen()) {
        setIrq(config_.uart, false);
        setHandler(config_.uart, nullptr);
        usart_disable(config_.uart);
        usart_deinit(config_.uart);
        setOpened(false);
        config_.uart = 0;
    }
}

int32_t Uart::write_(const void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || len == 0)
        return -1;

    // Fill transmit queue
    __disable_irq();
    const uint8_t *data = static_cast<const uint8_t*>(buf);
    const uint32_t available = config_.txQueue->available();
    const uint32_t size = available < len ? available : len;
    for (uint32_t i = 0; i < size; ++i) {
        config_.txQueue->push(data[i]);
    }

    // Start transmission if it is idle
    if (SET == usart_flag_get(config_.uart, USART_FLAG_TBE)) {
        usart_data_transmit(config_.uart, config_.txQueue->front());
        config_.txQueue->pop();
        usart_interrupt_enable(config_.uart, USART_INT_TBE);
    }
    __enable_irq();
    return size;
}

int32_t Uart::read_(void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || len == 0)
        return -1;

    // Fill receive buffer from queue
    __disable_irq();
    uint8_t *data = static_cast<uint8_t*>(buf);
    const uint32_t currSize = config_.rxQueue->size();
    const uint32_t size = currSize < len ? currSize : len;
    for (uint32_t i = 0; i < size; ++i) {
        data[i] = config_.rxQueue->front();
        config_.rxQueue->pop();
    }
    __enable_irq();
    return size;
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
                usart_baudrate_set(config_.uart, newBaud);
                return true;
            }
        }
        break;

    case kFlushInput:
        config_.rxQueue->clear();
        return true;

    case kFlushOutput: {
        bool empty = false;
        while (!empty) {
            __disable_irq();
            empty = config_.txQueue->empty();
            __enable_irq();
        }
        return true;
    }
    default:
        break;
    }

    return false;
}

void Uart::irqHandler(Uart* uart)
{
    // Check instance pointer
    if (!uart)
        return;

    // Receive data
    if (RESET != usart_interrupt_flag_get(uart->config_.uart, USART_INT_FLAG_RBNE)) {
        if (!uart->config_.rxQueue->full()) {
            uart->config_.rxQueue->push(usart_data_receive(uart->config_.uart));
        }
    }

    // Transmit data
    if (RESET != usart_interrupt_flag_get(uart->config_.uart, USART_INT_FLAG_TBE)) {
        if (uart->config_.txQueue->empty()) {
            usart_interrupt_disable(uart->config_.uart, USART_INT_TBE);
        } else {
            usart_data_transmit(uart->config_.uart, uart->config_.txQueue->front());
            uart->config_.txQueue->pop();
        }
    }
}

extern "C" void USART0_IRQHandler(void)
{
    Uart::irqHandler(uartInstances[0]);
}

extern "C" void USART1_IRQHandler(void)
{
    Uart::irqHandler(uartInstances[1]);
}

extern "C" void USART2_IRQHandler(void)
{
    Uart::irqHandler(uartInstances[2]);
}

extern "C" void UART3_IRQHandler(void)
{
    Uart::irqHandler(uartInstances[3]);
}

extern "C" void UART4_IRQHandler(void)
{
    Uart::irqHandler(uartInstances[4]);
}

#if defined(GD32F4XX_H)

extern "C" void USART5_IRQHandler(void)
{
    Uart::irqHandler(uartInstances[5]);
}

extern "C" void UART6_IRQHandler(void)
{
    Uart::irqHandler(uartInstances[6]);
}

extern "C" void UART7_IRQHandler(void)
{
    Uart::irqHandler(uartInstances[7]);
}

#endif

}; // namespace gd32

/***************************** END OF FILE ************************************/
