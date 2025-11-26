/*******************************************************************************
 * @file    debug.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of serial debug module.
 ******************************************************************************/

#pragma once

// Check debug messages protocol header and include it if exists
#if __has_include("debug_msg.h")
#include "debug_msg.h"
#endif

#include "periph/serialdrv.h"
#include "periph/gpio.h"

#ifndef DEBUG_BUFFER_SIZE
#define DEBUG_BUFFER_SIZE 32
#endif

/// @brief Callback for received debug data
typedef void (* DebugCallback)(const char* buf, uint32_t len);

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
    static void flush();

    static void out(uint8_t cmd, int8_t value);
    static void out(uint8_t cmd, int16_t value);
    static void out(uint8_t cmd, int32_t value);
    static void out(uint8_t cmd, float value);
    static void out(uint8_t cmd, const char* str);
    static void out(uint8_t cmd, const uint8_t* value, uint32_t len);

    static void outVersion();

    static void onTestPin();
    static void offTestPin();

    static void dispatcher();

private:
    static constexpr int kMsgSize = DEBUG_BUFFER_SIZE;
    static constexpr uint8_t kMsgFlag1 = 0x17;
    static constexpr uint8_t kMsgFlag2 = 0xAA;

    static bool enabled_;
    static uint32_t bufPos_;
    static char buf_[kMsgSize];
    static DebugCallback cb_;
    static SerialDrv* drv_;
    static Gpio* testPin_;

    Debug() = delete;
};

/***************************** END OF FILE ************************************/
