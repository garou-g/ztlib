/*******************************************************************************
 * @file    system.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Global system class functions.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <assert.h>

#include "system.hpp"
#include "systemproxy.hpp"
#include "attr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
 * @brief Global boot status structure for holding reset states
 */
RETAIN_NOINIT_ATTR System::BootStatus System::bootStatus;

/**
 * @brief Global object pointer for singleton operations
 */
System* System::system = nullptr;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Init singleton instance. MUST BE called before any actions with module
 */
void System::init()
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
    return hardware;
}

/**
 * @brief Returns reference to hardware version structure
 *
 * @return const FirmwareVersion& fw version structure
 */
const Version::Firmware& System::firmwareVersion() const
{
    return firmware;
}

/**
 * @brief Returns state of first boot of device
 *
 * @return true if device first time boots from power on, otherwise false
 */
bool System::isFirstStart() const
{
    return bootStatus.resetCounter == 1;
}

/**
 * @brief Returns count of resets since device power on
 *
 * @return uint16_t reset count
 */
uint16_t System::resetCounter() const
{
    return bootStatus.resetCounter;
}

/**
 * @brief Returns reason of last reset
 *
 * @return uint16_t last reset reason
 */
System::ResetReason System::resetReason() const
{
    return mResetReason;
}

/**
 * @brief Returns reason of last wakeup
 *
 * @return System::WakeupReason last wakeup reason
 */
System::WakeupReason System::wakeupReason() const
{
    return mWakeupReason;
}

/**
 * @brief Sets time for wakeup from sleep
 *
 * @param timeMs wakeup time in ms
 */
void System::setWakeupTime(uint32_t timeMs)
{
    mWakeupTime = timeMs;
}

/**
 * @brief Returns current wakeup from sleep time
 *
 * @return uint32_t current wakeup time in ms
 */
uint32_t System::wakeupTime() const
{
    return mWakeupTime;
}

/**
 * @brief Sets and enables wakeup by external pin
 *
 * @param pin wakeup pin
 */
void System::setWakeupPin(int32_t pin)
{
    mWakeupPin = pin;
}

/**
 * @brief Returns current wakeup from sleep pin
 *
 * @return int32_t current wakeup pin
 */
int32_t System::wakeupPin() const
{
    return mWakeupPin;
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Construct a new System object
 *
 * @param ver version instance
 */
System::System(Version* ver)
    : version(ver)
    , mResetReason(kResetUnknown)
    , mWakeupReason(kWakeupUnknown)
    , mWakeupTime(0)
    , mWakeupPin(-1)
{
    assert(version != nullptr);
    version->getHardwareVersion(hardware);
    version->getFirmwareVersion(firmware);
}

/**
 * @brief Sets last reset reason
 *
 * @param reset last reset reason
 */
void System::setResetReason(System::ResetReason reset)
{
    mResetReason = reset;
}

/**
 * @brief Sets last wakeup reason
 *
 * @param wakeup last wakeup reason
 */
void System::setWakeupReason(System::WakeupReason wakeup)
{
    mWakeupReason = wakeup;
}

/***************************** END OF FILE ************************************/
