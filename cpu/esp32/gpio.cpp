/*******************************************************************************
 * @file    gpio.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   ESP32 GPIO driver module.
 ******************************************************************************/

#include "gpio.h"

#include "driver/gpio.h"

#include <cassert>

bool Gpio::open(const void* drvConfig)
{
    if (isOpen())
        return false;

    assert(drvConfig != nullptr);
    const GpioConfig* config
        = static_cast<const GpioConfig*>(drvConfig);
    if (!GPIO_IS_VALID_OUTPUT_GPIO(config->pin))
        return false;

    port_ = 0;
    pin_ = config->pin;
    inverse_ = config->inverse;

    gpio_pad_select_gpio(pin_);
    gpio_set_direction(pin_, GPIO_MODE_OUTPUT);
    gpio_set_level(pin_, inverse_ ? 1: 0);
    gpio_hold_en(pin_);
    return true;
}

void Gpio::close()
{
    if (isOpen()) {
        gpio_hold_dis(pin_);
        gpio_reset_pin(pin_);
        port_ = -1;
        pin_ = -1;
        inverse_ = false;
    }
}

void Gpio::set()
{
    if (!isOpen())
        return;
    gpio_hold_dis(pin_);
    gpio_set_level(pin_, inverse_ ? 0: 1);
    gpio_hold_en(pin_);
}

void Gpio::reset()
{
    if (!isOpen())
        return;
    gpio_hold_dis(pin_);
    gpio_set_level(pin_, inverse_ ? 1: 0);
    gpio_hold_en(pin_);
}

bool Gpio::ioctl(uint32_t cmd, void* pValue)
{
    return true;
}

/***************************** END OF FILE ************************************/
