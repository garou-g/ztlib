/*******************************************************************************
 * @file    system.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Global system class functions.
 ******************************************************************************/

#include "system.hpp"
#include "systemproxy.hpp"
#include "attr.h"

#include <cassert>

/// Global system data structure that holds between any sleep states
RETAIN_NOINIT_ATTR System::SystemData System::systemData;

/// Global object pointer for singleton operations
System* System::system = nullptr;

/**
 * @brief Init singleton instance. MUST BE called before any actions with module
 */
void System::initInstance()
{
    if (system != nullptr)
        delete system;
    system = SystemProxy::initSystemByPlatform();
}

/**
 * @brief Returns global object instance
 *
 * @return System& reference to module object
 */
System& System::getInstance()
{
    assert(system != nullptr);
    return *system;
}

/**
 * @brief Returns reference to hardware version structure
 *
 * @return const HardwareVersion& hw version structure
 */
const Version::Hardware& System::hardwareVersion() const
{
    return systemData.hardware;
}

/**
 * @brief Returns reference to hardware version structure
 *
 * @return const FirmwareVersion& fw version structure
 */
const Version::Firmware& System::firmwareVersion() const
{
    return systemData.firmware;
}

/**
 * @brief Returns reference to firmware version string
 *
 * @return const char* fw version string
 */
const char* System::firmwareVersionString() const
{
    return versionStr;
}

/**
 * @brief Returns state of first boot of device
 *
 * @return true if device first time boots from power on, otherwise false
 */
bool System::isFirstStart() const
{
    return systemData.resetCounter == 1;
}

/**
 * @brief Returns count of resets since device power on
 *
 * @return uint16_t reset count
 */
uint16_t System::resetCounter() const
{
    return systemData.resetCounter;
}

/**
 * @brief Returns reason of last reset
 *
 * @return uint16_t last reset reason
 */
System::ResetReason System::resetReason() const
{
    return resetReason_;
}

/**
 * @brief Returns reason of last wakeup
 *
 * @return System::WakeupReason last wakeup reason
 */
System::WakeupReason System::wakeupReason() const
{
    return wakeupReason_;
}

/**
 * @brief Sets time for wakeup from sleep
 *
 * @param timeMs wakeup time in ms
 */
void System::setWakeupTime(uint32_t timeMs)
{
    wakeupTime_ = timeMs;
}

/**
 * @brief Returns current wakeup from sleep time
 *
 * @return uint32_t current wakeup time in ms
 */
uint32_t System::wakeupTime() const
{
    return wakeupTime_;
}

/**
 * @brief Sets and enables wakeup by external pin
 *
 * @param pin wakeup pin
 */
void System::setWakeupPin(int32_t pin)
{
    wakeupPin_ = pin;
}

/**
 * @brief Returns current wakeup from sleep pin
 *
 * @return int32_t current wakeup pin
 */
int32_t System::wakeupPin() const
{
    return wakeupPin_;
}

/**
 * @brief Construct a new System object
 *
 * @param ver version instance
 */
System::System(Version* ver)
    : version(ver)
    , versionStr { 0 }
    , resetReason_(kResetUnknown)
    , wakeupReason_(kWakeupUnknown)
    , wakeupTime_(0)
    , wakeupPin_(-1)
{
    assert(version != nullptr);

    // Reset states update
    if (systemData.firstStart != 0x55AA) {
        systemData.firstStart = 0x55AA;
        systemData.resetCounter = 1;

        // Hardware version readed only one time when first boot.
        // It never changes because it hardware feature
        version->getHardwareVersion(systemData.hardware);
    } else {
        systemData.resetCounter++;
    }

    // Software version always updates.
    // It changes when software update for example
    version->getFirmwareVersion(systemData.firmware, versionStr);
}

/**
 * @brief Sets last reset reason
 *
 * @param reset last reset reason
 */
void System::setResetReason(System::ResetReason reset)
{
    resetReason_ = reset;
}

/**
 * @brief Sets last wakeup reason
 *
 * @param wakeup last wakeup reason
 */
void System::setWakeupReason(System::WakeupReason wakeup)
{
    wakeupReason_ = wakeup;
}

/***************************** END OF FILE ************************************/
