/*******************************************************************************
 * @file    deviceproxy.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Device proxy class file.
 ******************************************************************************/

#include "deviceproxy.h"
#include "system.h"

/* Add device specific includes here -----------------------------------------*/
#include "dummy_device.h"

/**
 * @brief Reference realization of this method. Copy this file into your project
 *      and use concrete devices in return expressions.
 */
Device* DeviceProxy::initDeviceByVersion()
{
    const Version::Hardware& hw = System::getInstance().hardwareVersion();

    switch (hw.major1) {
    case 0:
        switch (hw.minor1) {
        default:
            return new DummyDevice(); // Device 0.x revision
        }

    case 3:
        switch (hw.minor1) {
        case 1:
            return new DummyDevice(); // Device 3.1 revision
        default:
            return new DummyDevice(); // Device 3.x revision
        }

    default:
        return new DummyDevice(); // Not defined device
    }
}

/***************************** END OF FILE ************************************/
