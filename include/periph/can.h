/*******************************************************************************
 * @file    can.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of CAN bus driver.
 ******************************************************************************/

#pragma once

#include "serialdrv.h"
#include "can_types.h"

/// @brief CAN bus peripheral driver
class Can : public SerialDrv {
public:
    enum IoctlCmd {
        kSetBaudrate,   // Sets CAN baudrate
        kSetFilterId,
        kSetMaskId,
    };

    /**
     * @brief Writes CAN data message to driver
     *
     * @param msg CAN message structure
     * @return int32_t >0 if success, -1 on error
     */
    virtual int32_t write(const CanMsg& msg) = 0;

    /**
     * @brief Reads CAN data message from driver
     *
     * @param msg CAN message structure
     * @return int32_t >0 if success, -1 on error
     */
    virtual int32_t read(CanMsg& msg) = 0;

private:
    // Don't use original serialdrv methods
    int32_t write_(const void* buf, uint32_t len) override { return -1; }
    int32_t read_(void* buf, uint32_t len) override { return -1; }

};

/***************************** END OF FILE ************************************/
