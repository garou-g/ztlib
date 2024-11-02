/*******************************************************************************
 * @file    version.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   GD32 version driver module.
 ******************************************************************************/

#include "version.h"

/**
 * @brief Read HW version voltages and return it
 *
 * @return Version::HwVolt structure with HW voltages
 */
Version::HwVolt Version::getHwValue()
{
    HwVolt hw;
    hw.major = 0;
    hw.minor = 0;
    return hw;
}

/**
 * @brief Read FW version data and return it
 *
 * @return Version::FwString structure with FW version
 */
Version::FwString Version::getFwValue()
{
    FwString fw = {
#if defined(VERSION_NUMBER)
        .data = VERSION_NUMBER
#else
        .data = "0.0.0"
#endif
    };
    return fw;
}

/***************************** END OF FILE ************************************/
