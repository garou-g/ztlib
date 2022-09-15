/*******************************************************************************
 * @file    gpio.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Abstract gpio driver.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "gpio.hpp"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

Gpio::Gpio()
    : opened(false)
{
}

bool Gpio::isOpen() const
{
    return opened;
}

void Gpio::setOpened(bool state)
{
    opened = state;
}

bool Gpio::isInversed() const
{
    return inversed;
}

void Gpio::setInversed(bool state)
{
    inversed = state;
}

/* Private functions ---------------------------------------------------------*/

/***************************** END OF FILE ************************************/
