/*******************************************************************************
 * @file    systemproxy.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   System proxy class file.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "systemproxy.hpp"

#include "espsystem.hpp"
#include "espversion.hpp"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief Choose correct system successor for main instance init
 *
 * @return System* new System instance by current platform
 */
System* SystemProxy::initSystemByPlatform()
{
#if defined(ESP_PLATFORM)
    return new EspSystem(new EspVersion());
#else
#error("Not defined platform for version definition")
#endif
}

/* Private functions ---------------------------------------------------------*/

/***************************** END OF FILE ************************************/
