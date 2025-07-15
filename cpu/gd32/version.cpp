/*******************************************************************************
 * @file    version.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   GD32 version driver module.
 ******************************************************************************/

#include "version.h"

/**
 * @brief Read HW version and return it
 *
 * @return Version::Hardware structure
 */
__attribute__((weak)) Version::Hardware Version::getHwValue()
{
    return { 0, 0 };
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
