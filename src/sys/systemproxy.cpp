/*******************************************************************************
 * @file    systemproxy.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   System proxy class file.
 ******************************************************************************/

#include "systemproxy.h"

#include "espsystem.h"

/**
 * @brief Choose correct system successor for main instance init
 *
 * @return System* new System instance by current platform
 */
System* SystemProxy::initSystemByPlatform()
{
#if defined(ESP_PLATFORM)
    return new EspSystem();
#else
#error("Not defined platform for version definition")
#endif
}

/***************************** END OF FILE ************************************/
