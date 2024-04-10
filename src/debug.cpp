/*******************************************************************************
 * @file    debug.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Serial debug module.
 ******************************************************************************/

#include "debug.h"
#include "attr.h"

#include <cassert>

RETAIN_NOINIT_ATTR bool Debug::enabled_;
DebugCallback Debug::cb_ = nullptr;
SerialDrv* Debug::drv_ = nullptr;

/**
 * @brief Inits debug module and sets callback if it exists
 *
 * @param debugCb debug callback for received data processing
 */
void Debug::init(DebugCallback debugCb)
{
    // Driver can be set before or after init or even not set
    enabled_ = true;
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
 * @brief Enables debug module
 */
void Debug::enable()
{
    assert(drv_ != nullptr);
    enabled_ = true;
}

/**
 * @brief Disables debug module
 */
void Debug::disable()
{
    assert(drv_ != nullptr);
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
 * @brief Dispatcher for processing received debug data
 */
void Debug::dispatcher()
{
    if (!enabled_)
        return;

    assert(drv_ != nullptr);
    uint8_t buf[kMsgSize];
    const int readed = drv_->read(buf, kMsgSize);
    if (readed > 0 && cb_) {
        cb_(buf, readed);
    }
}

/***************************** END OF FILE ************************************/
