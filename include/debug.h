/*******************************************************************************
 * @file    debug.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of serial debug module.
 ******************************************************************************/

#pragma once

#include "periph/serialdrv.h"
#include "periph/gpio.h"

#ifndef DEBUG_BUFFER_SIZE
#define DEBUG_BUFFER_SIZE 32
#endif

/// @brief Callback for received debug data
typedef void (* DebugCallback)(const uint8_t* buf, uint32_t len);

/// @brief Serial data debug module
class Debug {
public:
    static void init(DebugCallback debugCb = nullptr);
    static void setDriver(SerialDrv* drv);
    static void setTestPin(Gpio* testPin);

    static void enable();
    static void disable();
    static bool isEnabled();

    static void print(const char* buf);
    static void print(const char* buf, uint32_t len);

    static void onTestPin();
    static void offTestPin();

    static void dispatcher();

private:
    static constexpr int kMsgSize = DEBUG_BUFFER_SIZE;

    static bool enabled_;
    static uint32_t bufPos_;
    static uint8_t buf_[kMsgSize];
    static DebugCallback cb_;
    static SerialDrv* drv_;
    static Gpio* testPin_;

    Debug() = delete;
};

/***************************** END OF FILE ************************************/
