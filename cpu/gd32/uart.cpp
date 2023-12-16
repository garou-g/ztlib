/*******************************************************************************
 * @file    uart.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   GD32 UART driver module.
 ******************************************************************************/

#include "uart.h"
#include "gd32_types.h"

#include <cassert>

static Uart* uartInstances[8] = { nullptr };

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
#if defined(GD32F10X_HD)
    case UART3: irqType = UART3_IRQn; break;
    case UART4: irqType = UART4_IRQn; break;
#endif
#if defined(GD32F4XX_H)
    case USART5: irqType = USART5_IRQn; break;
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

bool Uart::open(const void* drvConfig)
{
    if (isOpen())
        return false;

    assert(drvConfig != nullptr);
    const gd32::UartConfig* config
        = static_cast<const gd32::UartConfig*>(drvConfig);
    const UartConfig* baseConfig
        = static_cast<const UartConfig*>(config->baseConf);
    if (baseConfig->uart == 0 || !isPortExist(baseConfig->uart)
        || baseConfig->txQueue == nullptr || baseConfig->rxQueue == nullptr)
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
    usart_interrupt_enable(conf_.uart, USART_INT_RBNE);
    setHandler(conf_.uart, this);
    setIrq(conf_.uart, true);
    setOpened(true);

    return true;
}

void Uart::close()
{
    if (isOpen()) {
        setIrq(conf_.uart, false);
        setHandler(conf_.uart, nullptr);
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

    // Fill transmit queue
    __disable_irq();
    const uint8_t *data = static_cast<const uint8_t*>(buf);
    const uint32_t available = conf_.txQueue->available();
    const uint32_t size = available < len ? available : len;
    for (uint32_t i = 0; i < size; ++i) {
        conf_.txQueue->push(data[i]);
        // usart_data_transmit(conf_.uart, data[i]);
        // while (RESET == usart_flag_get(conf_.uart, USART_FLAG_TBE));
    }

    // Start transmission if it is idle
    if (RESET == usart_flag_get(conf_.uart, USART_FLAG_TBE)) {
        usart_data_transmit(conf_.uart, conf_.txQueue->front());
        conf_.txQueue->pop();
        usart_interrupt_enable(conf_.uart, USART_INT_TBE);
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
    const uint32_t currSize = conf_.rxQueue->size();
    const uint32_t size = currSize < len ? currSize : len;
    for (uint32_t i = 0; i < size; ++i) {
        data[i] = conf_.rxQueue->front();
        conf_.rxQueue->pop();
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

void Uart::irqHandler(Uart* uart)
{
    // Check instance pointer
    if (!uart)
        return;

    // Receive data
    if (RESET != usart_interrupt_flag_get(uart->conf_.uart, USART_INT_FLAG_RBNE)) {
        if (!uart->conf_.rxQueue->full()) {
            uart->conf_.rxQueue->push(usart_data_receive(uart->conf_.uart));
        }
    }

    // Transmit data
    if (RESET != usart_interrupt_flag_get(uart->conf_.uart, USART_INT_FLAG_TBE)) {
        if (uart->conf_.txQueue->empty()) {
            usart_interrupt_disable(uart->conf_.uart, USART_INT_TBE);
        } else {
            usart_data_transmit(uart->conf_.uart, uart->conf_.txQueue->front());
            uart->conf_.txQueue->pop();
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

/***************************** END OF FILE ************************************/
