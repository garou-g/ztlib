/*******************************************************************************
 * @file    systemproxy.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of system proxy class.
 ******************************************************************************/

#pragma once

#include "system.h"

/**
 * @brief Proxy class for handle different version of System modules and
 *      construct it in order to current platform type
 */
class SystemProxy {
public:
    static System* initSystemByPlatform();
};

/***************************** END OF FILE ************************************/
