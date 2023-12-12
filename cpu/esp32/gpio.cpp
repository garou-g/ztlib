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
    const GpioConfig* config = static_cast<const GpioConfig*>(drvConfig);
    if (!GPIO_IS_VALID_OUTPUT_GPIO(config->pin))
        return false;

    port_ = 0;
    pin_ = config->pin;
    inverse_ = config->inverse;

    const gpio_num_t pin = static_cast<gpio_num_t>(pin_);
    gpio_pad_select_gpio(pin_);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, inverse_ ? 1: 0);
    gpio_hold_en(pin);
    return true;
}

void Gpio::close()
{
    if (isOpen()) {
        const gpio_num_t pin = static_cast<gpio_num_t>(pin_);
        gpio_hold_dis(pin);
        gpio_reset_pin(pin);
        port_ = -1;
        pin_ = -1;
        inverse_ = false;
    }
}

void Gpio::set()
{
    if (!isOpen())
        return;
    const gpio_num_t pin = static_cast<gpio_num_t>(pin_);
    gpio_hold_dis(pin);
    gpio_set_level(pin, inverse_ ? 0: 1);
    gpio_hold_en(pin);
}

void Gpio::reset()
{
    if (!isOpen())
        return;
    const gpio_num_t pin = static_cast<gpio_num_t>(pin_);
    gpio_hold_dis(pin);
    gpio_set_level(pin, inverse_ ? 1: 0);
    gpio_hold_en(pin);
}

bool Gpio::get() const
{
    if (!isOpen())
        return false;

    const gpio_num_t pin = static_cast<gpio_num_t>(pin_);
    const bool res = gpio_get_level(pin) == 1;
    return inverse_ ? !res : res;
}

bool Gpio::ioctl(uint32_t cmd, void* pValue)
{
    return true;
}

/***************************** END OF FILE ************************************/
