/*******************************************************************************
 * @file    debug.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Serial debug module.
 ******************************************************************************/

#include "debug.h"
#include "attr.h"
#include "crc.h"

#include <cassert>

RETAIN_NOINIT_ATTR bool Debug::enabled_;
uint32_t Debug::bufPos_ = 0;
char Debug::buf_[kMsgSize] = {};
DebugCallback Debug::cb_ = nullptr;
SerialDrv* Debug::drv_ = nullptr;
Gpio* Debug::testPin_ = nullptr;

/**
 * @brief Inits debug module and sets callback if it exists
 *
 * @param debugCb debug callback for received data processing
 */
void Debug::init(DebugCallback debugCb)
{
    // Driver can be set before or after init or even not set
    enabled_ = true;
    bufPos_ = 0;
    buf_[bufPos_] = '\0';
    cb_ = debugCb;
}

/**
 * @brief Sets serial driver for debug output
 *
 * @param drv driver for data output
 */
void Debug::setDriver(SerialDrv* drv)
{
    assert(drv != nullptr);
    drv_ = drv;
}

/**
 * @brief Sets test pin instance
 *
 * @param testPin test pin gpio
 */
void Debug::setTestPin(Gpio* testPin)
{
    assert(testPin != nullptr);
    testPin_ = testPin;
}

/**
 * @brief Enables debug module
 */
void Debug::enable()
{
    enabled_ = true;
}

/**
 * @brief Disables debug module
 */
void Debug::disable()
{
    enabled_ = false;
}

/**
 * @brief Returns current state of debug module
 *
 * @return true if enabled otherwise false
 */
bool Debug::isEnabled()
{
    return enabled_;
}

/**
 * @brief Prints debug data from buf. Must be zero ended
 *
 * @param buf data buffer
 */
void Debug::print(const char* buf)
{
    if (!enabled_)
        return;

    assert(drv_ != nullptr);
    uint32_t len = 0;
    while (buf[len] != 0)
        ++len;
    drv_->write(buf, len);
}

/**
 * @brief Prints len of debug data from buf
 *
 * @param buf data buffer
 * @param len data length
 */
void Debug::print(const char* buf, uint32_t len)
{
    if (!enabled_)
        return;

    assert(drv_ != nullptr);
    drv_->write(buf, len);
}

void Debug::out(uint8_t cmd, int8_t value)
{
    if (!enabled_)
        return;

    assert(drv_ != nullptr);
    const uint8_t len = sizeof(cmd) + sizeof(value);
    uint8_t* pValue = (uint8_t*)&value;
    uint8_t buf[] = {
        kMsgFlag1,
        kMsgFlag2,
        len,
        cmd,
        pValue[0],
        0,
    };
    buf[sizeof(buf) - 1] = crc8(&buf[3], len);
    drv_->write(buf, sizeof(buf));
}

void Debug::out(uint8_t cmd, int16_t value)
{
    if (!enabled_)
        return;

    assert(drv_ != nullptr);
    const uint8_t len = sizeof(cmd) + sizeof(value);
    uint8_t* pValue = (uint8_t*)&value;
    uint8_t buf[] = {
        kMsgFlag1,
        kMsgFlag2,
        len,
        cmd,
        pValue[0],
        pValue[1],
        0,
    };
    buf[sizeof(buf) - 1] = crc8(&buf[3], len);
    drv_->write(buf, sizeof(buf));
}

void Debug::out(uint8_t cmd, int32_t value)
{
    if (!enabled_)
        return;

    assert(drv_ != nullptr);
    const uint8_t len = sizeof(cmd) + sizeof(value);
    uint8_t* pValue = (uint8_t*)&value;
    uint8_t buf[] = {
        kMsgFlag1,
        kMsgFlag2,
        len,
        cmd,
        pValue[0],
        pValue[1],
        pValue[2],
        pValue[3],
        0,
    };
    buf[sizeof(buf) - 1] = crc8(&buf[3], len);
    drv_->write(buf, sizeof(buf));
}

void Debug::out(uint8_t cmd, float value)
{
    if (!enabled_)
        return;

    assert(drv_ != nullptr);
    const uint8_t len = sizeof(cmd) + sizeof(value);
    uint8_t* pValue = (uint8_t*)&value;
    uint8_t buf[] = {
        kMsgFlag1,
        kMsgFlag2,
        len,
        pValue[0],
        pValue[1],
        pValue[2],
        pValue[3],
        0,
    };
    buf[sizeof(buf) - 1] = crc8(&buf[3], len);
    drv_->write(buf, sizeof(buf));
}

void Debug::out(uint8_t cmd, const char* str)
{
    if (!enabled_)
        return;

    assert(drv_ != nullptr);
    uint32_t len = 0;
    while (str[len] != 0)
        ++len;

    uint8_t buf[5 + len];
    buf[0] = kMsgFlag1;
    buf[1] = kMsgFlag2;
    buf[2] = len + sizeof(cmd);
    buf[3] = cmd;
    for (uint32_t i = 0; i < len; ++i) {
        buf[4 + i] = str[i];
    }
    buf[sizeof(buf) - 1] = crc8(&buf[3], len + sizeof(cmd));
    drv_->write(buf, sizeof(buf));
}

/**
 * @brief Sets test pin to high
 */
void Debug::onTestPin()
{
    if (!testPin_ || !enabled_)
        return;
    testPin_->set();
}

/**
 * @brief Sets test pin to low
 */
void Debug::offTestPin()
{
    if (!testPin_ || !enabled_)
        return;
    testPin_->reset();
}

/**
 * @brief Dispatcher for processing received debug data
 */
void Debug::dispatcher()
{
    if (!enabled_)
        return;

    assert(drv_ != nullptr);

    // Process only if callback exists
    if (cb_) {
        char tmp[kMsgSize];
        const int readed = drv_->read(tmp, kMsgSize);

        // If data was readed - check end of line or just add to internal buf
        if (readed > 0) {
            bool eol = false;
            for (uint32_t i = 0; i < readed; ++i) {
                if (tmp[i] == '\n') {
                    buf_[bufPos_++] = '\0';
                    eol = true;
                    break;
                } else {
                    buf_[bufPos_++] = tmp[i];
                    if (bufPos_ >= kMsgSize) {
                        bufPos_ = 0;
                    }
                }
            }

            // End of line found - proccess callback
            if (eol) {
                cb_(buf_, bufPos_);
                bufPos_ = 0;
            }
        }
    }
}

/***************************** END OF FILE ************************************/
