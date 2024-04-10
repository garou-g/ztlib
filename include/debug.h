/*******************************************************************************
 * @file    debug.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of serial debug module.
 ******************************************************************************/

#pragma once

#include "periph/serialdrv.h"

/// @brief Callback for received debug data
typedef void (* DebugCallback)(const uint8_t* buf, uint32_t len);

/// @brief Serial data debug module
class Debug {
public:
    static void init(DebugCallback debugCb = nullptr);
    static void setDriver(SerialDrv* drv);

    static void enable();
    static void disable();
    static bool isEnabled();

    static void print(const char* buf);
    static void print(const char* buf, uint32_t len);

    static void dispatcher();

private:
    static constexpr int kMsgSize = 32;

    static bool enabled_;
    static DebugCallback cb_;
    static SerialDrv* drv_;

    Debug() = delete;
};

/***************************** END OF FILE ************************************/
