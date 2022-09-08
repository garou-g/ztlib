/*******************************************************************************
 * @file    espgpio.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   ESP32 GPIO driver module.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <cassert>

#include "espgpio.hpp"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief Default empty constructor
 */
EspGpio::EspGpio()
    : Gpio()
    , pin(GPIO_NUM_NC)
{
}

bool EspGpio::open(const void* const drvConfig)
{
    if (isOpen())
        return false;

    assert(drvConfig != nullptr);
    const Gpio::Config* const config
        = static_cast<const Gpio::Config*>(drvConfig);
    if (!GPIO_IS_VALID_OUTPUT_GPIO(config->pin))
        return false;
    pin = static_cast<gpio_num_t>(config->pin);

    gpio_pad_select_gpio(pin);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
    gpio_hold_en(pin);
    setOpened(true);

    return true;
}

void EspGpio::close()
{
    if (isOpen()) {
        gpio_hold_dis(pin);
        gpio_reset_pin(pin);
        setOpened(false);
        pin = GPIO_NUM_NC;
    }
}

int32_t EspGpio::set()
{
    if (!isOpen())
        return -1;

    gpio_hold_dis(pin);
    gpio_set_level(pin, 1);
    gpio_hold_en(pin);

    return 0;
}

int32_t EspGpio::reset()
{
    if (!isOpen())
        return -1;

    gpio_hold_dis(pin);
    gpio_set_level(pin, 0);
    gpio_hold_en(pin);

    return 0;
}

bool EspGpio::ioctl(uint32_t cmd, void* const pValue)
{
    if (!isOpen())
        return false;

    switch (static_cast<IoctlCmd>(cmd)) {
    default:
        break;
    }

    return false;
}

/* Private functions ---------------------------------------------------------*/

/***************************** END OF FILE ************************************/
