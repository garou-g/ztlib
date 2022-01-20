/*******************************************************************************
 * @file    periphdrv.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Abstract peripheral driver.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "periphdrv.hpp"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

PeriphDrv::PeriphDrv()
    : opened(false)
    , reg(-1)
{
}

bool PeriphDrv::isOpen() const
{
    return opened;
}

int32_t PeriphDrv::write(uint8_t reg, const void* buf, uint32_t len)
{
    this->reg = reg;
    int32_t res = write(buf, len);
    this->reg = -1; // Mark that no actual register value after writing
    return res;
}

int32_t PeriphDrv::read(uint8_t reg, void* buf, uint32_t len)
{
    this->reg = reg;
    int32_t res = read(buf, len);
    this->reg = -1; // Mark that no actual register value after writing
    return res;
}

void PeriphDrv::setOpened(bool state)
{
    opened = state;
}

int32_t PeriphDrv::getReg() const
{
    return reg;
}

/* Private functions ---------------------------------------------------------*/

/***************************** END OF FILE ************************************/
