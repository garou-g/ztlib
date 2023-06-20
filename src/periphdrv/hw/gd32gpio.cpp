/*******************************************************************************
 * @file    gd32gpio.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   GD32 GPIO driver module.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "gd32gpio.hpp"

#include <cassert>

/**
 * @brief Default empty constructor
 */
Gd32Gpio::Gd32Gpio()
    : Gpio()
    , port(0)
    , pin(-1)
{
}

bool Gd32Gpio::open(const void* drvConfig)
{
    if (isOpen())
        return false;

    assert(drvConfig != nullptr);
    const Gd32Gpio::Config* config
        = static_cast<const Gd32Gpio::Config*>(drvConfig);
    if ((config->pin & GPIO_PIN_ALL) == 0 || !isPortExist(config->port))
        return false;
    port = config->port;
    pin = config->pin;
    setInversed(config->inverse);

    rcu_periph_clock_enable(config->clock);
    gpio_init(port, config->mode, GPIO_OSPEED_50MHZ, pin);
    gpio_bit_write(port, pin, isInversed() ? SET : RESET);
    setOpened(true);

    return true;
}

void Gd32Gpio::close()
{
    if (isOpen()) {
        gpio_bit_write(port, pin, isInversed() ? SET : RESET);
        setOpened(false);
        port = 0;
        pin = -1;
    }
}

int32_t Gd32Gpio::set()
{
    if (!isOpen())
        return -1;

    gpio_bit_write(port, pin, isInversed() ? RESET : SET);

    return 0;
}

int32_t Gd32Gpio::reset()
{
    if (!isOpen())
        return -1;

    gpio_bit_write(port, pin, isInversed() ? SET : RESET);

    return 0;
}

bool Gd32Gpio::ioctl(uint32_t cmd, void* pValue)
{
    if (!isOpen())
        return false;

    switch (static_cast<IoctlCmd>(cmd)) {
    default:
        break;
    }

    return false;
}

/**
 * @brief Checks correctness of chosen gpio port
 *
 * @param port gpio port
 * @return true if port value is correct otherwise false
 */
bool Gd32Gpio::isPortExist(uint32_t port) const
{
    return port == GPIOA || port == GPIOB
        || port == GPIOC || port == GPIOD
        || port == GPIOE || port == GPIOF
        || port == GPIOG;
}

/***************************** END OF FILE ************************************/
