/*******************************************************************************
 * @file    can.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of CAN bus driver.
 ******************************************************************************/

#pragma once

#include "serialdrv.h"
// #include "can_types.h"

/// @brief CAN bus peripheral driver
class Can : public SerialDrv {
public:
    enum IoctlCmd {
        kSetBaudrate,   // Sets CAN baudrate
    };
};

/***************************** END OF FILE ************************************/
