/*******************************************************************************
 * @file    debug.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Serial debug module.
 ******************************************************************************/

#include "debug.h"
#include "attr.h"

#include <cassert>

RETAIN_NOINIT_ATTR bool Debug::enabled_;
uint32_t Debug::bufPos_ = 0;
uint8_t Debug::buf_[kMsgSize] = {};
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
        uint8_t tmp[kMsgSize];
        const int readed = drv_->read(tmp, kMsgSize);

        // If data was readed - check end of line or just add to internal buf
        if (readed > 0) {
            bool eol = false;
            for (uint32_t i = 0; i < readed; ++i) {
                if (tmp[i] == '\n') {
                    // len += i;
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
