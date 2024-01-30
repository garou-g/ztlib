/*******************************************************************************
 * @file    gpio.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   GD32 GPIO driver module.
 ******************************************************************************/

#include "periph/gpio.h"
#include "gd32/gd32_types.h"

#include <cassert>

using namespace gd32;

/**
 * @brief Checks correctness of chosen gpio port
 *
 * @param port gpio port
 * @return true if port value is correct otherwise false
 */
static bool isPortExist(uint32_t port)
{
    return port == GPIOA || port == GPIOB
        || port == GPIOC || port == GPIOD
        || port == GPIOE || port == GPIOF
        || port == GPIOG
#if defined(GD32F4XX_H)
        || port == GPIOH || port == GPIOI
#endif
        ;
}

/**
 * @brief Enables clock of chosen gpio port
 *
 * @param port gpio port
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

namespace gd32 {

/**
 * @brief Helper function to init gpio pin with chosen configuration
 *
 * @param conf gpio pin configuration
 */
void initGpioPeriph(const gd32::GpioConfig* conf)
{
    if (conf->port == 0)
        return;

    enableClock(conf->port);
#if defined(GD32F4XX_H)
    gpio_mode_set(conf->port, conf->mode, conf->pull, conf->pin);
    if (conf->mode == GPIO_MODE_AF)
        gpio_af_set(conf->port, conf->function, conf->pin);
    if (conf->mode == GPIO_MODE_AF || conf->mode == GPIO_MODE_OUTPUT)
        gpio_output_options_set(conf->port, conf->outputType, GPIO_OSPEED_50MHZ, conf->pin);
#else
    gpio_init(conf->port, conf->mode, GPIO_OSPEED_50MHZ, conf->pin);
#endif
}

/**
 * @brief Helper function to deinit gpio pin
 *
 * @param conf gpio pin configuration
 */
void deinitGpioPeriph(const gd32::GpioConfig* conf)
{
    if (conf->port == 0)
        return;

#if defined(GD32F4XX_H)
    gpio_mode_set(conf->port, GPIO_MODE_INPUT, GPIO_PUPD_NONE, conf->pin);
#else
    gpio_init(conf->port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, conf->pin);
#endif
}

}; // namespace gd32

bool Gpio::open(const void* drvConfig)
{
    if (isOpen())
        return false;

    assert(drvConfig != nullptr);
    const gd32::GpioConfig* config
        = static_cast<const gd32::GpioConfig*>(drvConfig);
    if ((config->pin & GPIO_PIN_ALL) == 0 || !isPortExist(config->port))
        return false;

    initGpioPeriph(config);
    conf_.port = config->port;
    conf_.pin = config->pin;
    gpio_bit_write(conf_.port, conf_.pin, config->initHigh ? SET : RESET);
    return true;
}

void Gpio::close()
{
    if (isOpen()) {
        // gpio_bit_write(conf_.port, conf_.pin, RESET);
        conf_.port = -1;
        conf_.pin = -1;
    }
}

void Gpio::set()
{
    if (!isOpen())
        return;
    gpio_bit_write(conf_.port, conf_.pin, SET);
}

void Gpio::reset()
{
    if (!isOpen())
        return;
    gpio_bit_write(conf_.port, conf_.pin, RESET);
}

bool Gpio::get() const
{
    if (!isOpen())
        return false;

    return gpio_input_bit_get(conf_.port, conf_.pin) == SET;
}

bool Gpio::ioctl(uint32_t cmd, void* pValue)
{
    return true;
}

/***************************** END OF FILE ************************************/
