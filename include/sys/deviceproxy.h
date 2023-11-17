/*******************************************************************************
 * @file    deviceproxy.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of device proxy class.
 ******************************************************************************/

#pragma once

#include "device.h"

/**
 * @brief Proxy class for handle different version of Device modules and
 *      construct it in order to current hardware version.
 *
 *      Here defined only interface. Realization (i.e. deviceproxy.cpp) needs
 *      to be written in for concrete project.
 */
class DeviceProxy {
public:

    /**
     * @brief Choose correct device successor for main instance
     *
     * @return Device* new Device instance by version of hardware
     */
    static Device* initDeviceByVersion();
};

/***************************** END OF FILE ************************************/
