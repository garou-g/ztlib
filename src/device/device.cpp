/*******************************************************************************
 * @file    device.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Device module with main loop.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <cassert>

#include "device.hpp"
#include "deviceproxy.hpp"
#include "system.hpp"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/// Device instance pointer that inited every launch and wakeup by hw version
Device* Device::device_ = nullptr;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Init singleton instance. MUST BE called before any actions with module
 */
void Device::initInstance()
{
    // Init device by hardware version with DeviceProxy
    if (device_ != nullptr)
        delete device_;
    device_ = DeviceProxy::initDeviceByVersion();
    assert(device_ != nullptr);

#if defined(FREERTOS_USED)
    // Launch main task in FreeRTOS mode
    device_->taskInit("deviceTask", 16384, kDefaultPrior);
#endif
}

/**
 * @brief Returns global object instance
 *
 * @return Device& reference to module object
 */
Device& Device::getInstance()
{
    assert(device_ != nullptr);
    return *device_;
}

/**
 * @brief Returns device specific version code
 *
 * @return uint32_t device version code
 */
uint32_t Device::versionCode() const
{
    return versionCode_;
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Default device constructor with basic initialization
 *
 * @param versionCode device version code
 * @param defDispathDelayMs default dispatcher frequency
 */
Device::Device(uint32_t versionCode, uint32_t defDispathDelayMs)
    : state_(kInit)
    , versionCode_(versionCode)
    , dispatchDelayMs_(defDispathDelayMs)
{
}

/**
 * @brief Overrided dispatcher
 *
 * @return Time delay for before next call of this dispatcher
 */
Time Device::_dispatcher()
{
    Time delay = Time(0, 0, dispatchDelayMs_);

    switch (state_) {
    // Init state
    case kInit: {
        // Get system information about start reason
        System& system = System::getInstance();
        const System::ResetReason resetReason = system.resetReason();

        // Check start reason
        switch (resetReason) {
        case System::kResetSleep:
            initWakeup();
            break;
        case System::kResetBrownout:
            initPowerLoss();
            break;
        default:
            initReset();
            break;
        }

        state_ = kInited;
        break;
    }

    // Main cycle processing
    case kInited:
        dispatch();
        break;

    // Return to init if state unknown
    default:
        state_ = kInit;
        break;
    }

    return delay;
}

/***************************** END OF FILE ************************************/
