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
    const GpioConfig* config
        = static_cast<const GpioConfig*>(drvConfig);
    if ((config->pin & GPIO_PIN_ALL) == 0 || !isPortExist(config->port))
        return false;
    config_ = new GpioConfig();
    *config_ = *config;

    rcu_periph_clock_enable(config_->clock);
    gpio_init(config_->port, config_->mode, GPIO_OSPEED_50MHZ, config_->pin);
    gpio_bit_write(config_->port, config_->pin, config_->inverse ? SET : RESET);
    return true;
}

void Gpio::close()
{
    if (isOpen()) {
        gpio_bit_write(config_->port, config_->pin, config_->inverse ? SET : RESET);
        delete config_;
        config_ == nullptr;
    }
}

void Gpio::set()
{
    if (!isOpen())
        return;
    gpio_bit_write(config_->port, config_->pin, config_->inverse ? RESET : SET);
}

void Gpio::reset()
{
    if (!isOpen())
        return;
    gpio_bit_write(config_->port, config_->pin, config_->inverse ? SET : RESET);
}

bool Gpio::ioctl(uint32_t cmd, void* pValue)
{
    return true;
}

/***************************** END OF FILE ************************************/
