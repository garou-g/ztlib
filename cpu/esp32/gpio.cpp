/*******************************************************************************
 * @file    gpio.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   ESP32 GPIO driver module.
 ******************************************************************************/

#include "gpio.h"

#include "driver/gpio.h"

#include <cassert>

namespace esp32 {

static bool checkConfig(const GpioConfig* config)
{
    assert(config != nullptr);

    return GPIO_IS_VALID_OUTPUT_GPIO(config->pin);
}

bool Gpio::setConfig(const void* drvConfig)
{
    if (isOpen())
        return false;

    const GpioConfig* config = static_cast<const GpioConfig*>(drvConfig);
    if (!checkConfig(config)) {
        return false;
    }
    config_ = *config;
    return true;
}

bool Gpio::open()
{
    if (isOpen())
        return false;

    if (!checkConfig(&config_)) {
        return false;
    }

    const gpio_num_t pin = static_cast<gpio_num_t>(config_.pin);
    gpio_pad_select_gpio(config_.pin);
    gpio_mode_t mode;
    switch (config_.dir) {
    default:
    case GpioDir::Disabled: mode = GPIO_MODE_DISABLE; break;
    case GpioDir::Input: mode = GPIO_MODE_INPUT; break;
    case GpioDir::Output: mode = GPIO_MODE_OUTPUT; break;
    case GpioDir::InputOutput: mode = GPIO_MODE_INPUT_OUTPUT; break;
    }
    gpio_set_direction(pin, mode);
    if (config_.dir == GpioDir::Output
        || config_.dir == GpioDir::InputOutput) {
        gpio_set_level(pin, 0);
        gpio_hold_en(pin);
    }
    setOpened(true);
    return true;
}

void Gpio::close()
{
    if (isOpen()) {
        const gpio_num_t pin = static_cast<gpio_num_t>(config_.pin);
        if (config_.dir == GpioDir::Output
            || config_.dir == GpioDir::InputOutput) {
            gpio_hold_dis(pin);
            gpio_reset_pin(pin);
        }
        setOpened(false);
    }
}

void Gpio::set()
{
    if (!isOpen())
        return;

    if (config_.dir == GpioDir::Output
        || config_.dir == GpioDir::InputOutput) {
        const gpio_num_t pin = static_cast<gpio_num_t>(config_.pin);
        gpio_hold_dis(pin);
        gpio_set_level(pin, 1);
        gpio_hold_en(pin);
    }
}

void Gpio::reset()
{
    if (!isOpen())
        return;

    if (config_.dir == GpioDir::Output
        || config_.dir == GpioDir::InputOutput) {
        const gpio_num_t pin = static_cast<gpio_num_t>(config_.pin);
        gpio_hold_dis(pin);
        gpio_set_level(pin, 0);
        gpio_hold_en(pin);
    }
}

bool Gpio::get() const
{
    if (!isOpen())
        return false;

    const gpio_num_t pin = static_cast<gpio_num_t>(config_.pin);
    return gpio_get_level(pin) == 1;
}

bool Gpio::ioctl(uint32_t cmd, void* pValue)
{
    return true;
}

}; // namespace gd32

/***************************** END OF FILE ************************************/
