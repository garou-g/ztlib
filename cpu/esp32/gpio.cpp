/*******************************************************************************
 * @file    gpio.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   ESP32 GPIO driver module.
 ******************************************************************************/

#include "gpio.h"

#include "driver/gpio.h"
#include "esp_idf_version.h"

#include <cassert>

namespace esp32 {

static bool checkConfig(const GpioConfig* config)
{
    assert(config != nullptr);

    return GPIO_IS_VALID_GPIO(config->pin);
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

#if (ESP_IDF_VERSION_MAJOR == 5)
    gpio_config_t ioConf = {};
    ioConf.pin_bit_mask =  1 << config_.pin;
    switch (config_.dir) {
    default:
    case GpioDir::Disabled: ioConf.mode = GPIO_MODE_DISABLE; break;
    case GpioDir::Input: ioConf.mode = GPIO_MODE_INPUT; break;
    case GpioDir::Output: ioConf.mode = GPIO_MODE_OUTPUT; break;
    case GpioDir::InputOutput: ioConf.mode = GPIO_MODE_INPUT_OUTPUT; break;
    }
    ioConf.pull_up_en = GPIO_PULLUP_DISABLE;
    ioConf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    ioConf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&ioConf);
#else
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
#endif
    setOpened(true);
    return true;
}

void Gpio::close()
{
    if (isOpen()) {
        const gpio_num_t pin = static_cast<gpio_num_t>(config_.pin);
        if (config_.dir == GpioDir::Output
            || config_.dir == GpioDir::InputOutput) {
#if (ESP_IDF_VERSION_MAJOR != 5)
            gpio_hold_dis(pin);
#endif
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
#if (ESP_IDF_VERSION_MAJOR == 5)
        gpio_set_level(pin, 1);
#else
        gpio_hold_dis(pin);
        gpio_set_level(pin, 1);
        gpio_hold_en(pin);
#endif
    }
}

void Gpio::reset()
{
    if (!isOpen())
        return;

    if (config_.dir == GpioDir::Output
        || config_.dir == GpioDir::InputOutput) {
        const gpio_num_t pin = static_cast<gpio_num_t>(config_.pin);
#if (ESP_IDF_VERSION_MAJOR == 5)
        gpio_set_level(pin, 0);
#else
        gpio_hold_dis(pin);
        gpio_set_level(pin, 0);
        gpio_hold_en(pin);
#endif
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
