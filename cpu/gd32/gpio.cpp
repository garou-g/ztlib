/*******************************************************************************
 * @file    gpio.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   GD32 GPIO driver module.
 ******************************************************************************/

#include "gpio.h"
#include "gd32_types.h"

#include <cassert>

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
        || port == GPIOG;
}

bool Gpio::open(const void* drvConfig)
{
    if (isOpen())
        return false;

    assert(drvConfig != nullptr);
    const gd32::GpioConfig* config
        = static_cast<const gd32::GpioConfig*>(drvConfig);
    if ((config->pin & GPIO_PIN_ALL) == 0 || !isPortExist(config->port))
        return false;

    port_ = config->port;
    pin_ = config->pin;
    inverse_ = config->inverse;

    rcu_periph_clock_enable(config->clock);
    gpio_init(port_, config->mode, GPIO_OSPEED_50MHZ, pin_);
    gpio_bit_write(port_, pin_, inverse_ ? SET : RESET);
    return true;
}

void Gpio::close()
{
    if (isOpen()) {
        gpio_bit_write(port_, pin_, inverse_ ? SET : RESET);
        port_ = -1;
        pin_ = -1;
        inverse_ = false;
    }
}

void Gpio::set()
{
    if (!isOpen())
        return;
    gpio_bit_write(port_, pin_, inverse_ ? RESET : SET);
}

void Gpio::reset()
{
    if (!isOpen())
        return;
    gpio_bit_write(port_, pin_, inverse_ ? SET : RESET);
}

bool Gpio::get() const
{
    if (!isOpen())
        return false;

    const bool res = gpio_input_bit_get(port_, pin_) == SET;
    return inverse_ ? !res : res;
}

bool Gpio::ioctl(uint32_t cmd, void* pValue)
{
    return true;
}

/***************************** END OF FILE ************************************/
