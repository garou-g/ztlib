/*******************************************************************************
 * @file    module.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Base class for other modules with time and event functions.
 ******************************************************************************/

#include "module.h"

#if defined(FREERTOS_USED)
/// Default priority for module tasks
const UBaseType_t Module::kDefaultPrior = tskIDLE_PRIORITY + 10;
#endif

const int Module::kSuspended = 0x1;
const int Module::kAvailability = 0x2;
const int Module::kInited = 0x55AA0000;

/**
 * @brief Construct a new Module object
 */
Module::Module()
#if defined(FREERTOS_USED)
    : xHandle_(NULL)
#endif
{
    if (!isInited()) {
        flags_ = kInited | kAvailability;
        nextCallTime_ = 0;
    }
}

#if defined(FREERTOS_USED)
/**
 * @brief FreeRTOS ONLY. Construct a new Module object with
 *      internal task initialization. Also calls init inside because
 *      task starts right after init
 *
 * @param name human readable task name
 * @param stack task stack size
 * @param prior task priority
 */
Module::Module(const char* name, uint32_t stack, UBaseType_t prior)
    : xHandle_(NULL)
{
    if (!isInited()) {
        flags_ = kInited | kAvailability;
        nextCallTime_ = 0;
    }
    taskInit(name, stack, prior);
}

/**
 * @brief FreeRTOS ONLY. Internal task initialization function for later
 *      task start
 *
 * @param name human readable task name
 * @param stack task stack size
 * @param prior task priority
 */
void Module::taskInit(const char* name, uint32_t stack, UBaseType_t prior)
{
    if (xHandle_ == NULL)
        xTaskCreate(task, name, stack, this, prior, &xHandle_);
}
#endif

/**
 * @brief Performs reset module to default state
 */
void Module::reset()
{
    flags_ = kInited | kAvailability;
    nextCallTime_ = 0;
}

/**
 * @brief Returns init state of module. Can be used for prevent reinit when
 *      used in retain memory and device restarts
 *
 * @return true if module was already inited otherwise false
 */
bool Module::isInited() const
{
    return (flags_ & kInited) == kInited;
}

/**
 * @brief Returns availability of the module to use. Is it configured correctly,
 *      has all drivers installed, etc. For example, in device-specific
 *      cases where some modules are not available on the current hardware,
 *      but the software still references those modules. Can be used inside or
 *      outside the module
 *
 * @return true if module can be used, otherwise false
 */
bool Module::isAvailable() const
{
    return (flags_ & kAvailability) == kAvailability;
}

/**
 * @brief Returns the Time object to wait for the next call to this
 *      object's dispatcher
 *
 * @return const Time new call delay time
 */
const Time Module::delayTime() const
{
    if (nextCallTime_.isZero())
        return 0;
    else {
        Time delta = nextCallTime_ - Time::now();
        if (delta < Time(0, 0, 0))
            return 0;
        else
            return delta;
    }
}

/**
 * @brief Returns the Time object absolute time for the next call to this
 *      object's dispatcher.
 *
 * @return const Time& new call absolute time
 */
const Time& Module::nextCallTime() const
{
    return nextCallTime_;
}

/**
 * @brief Module dispatcher function. Calls the virtual dispatch function
 *      to actually work in the successor classes.
 */
void Module::dispatcher()
{
    // If suspended - no processing, just return
    if (isSuspended())
        return;

    // If not available - no processing, suspend and return
    if (!isAvailable()) {
        suspend();
        return;
    }

    // Dispatcher called only if was zero delay or time has come
    Time now = Time::now();
    if (nextCallTime_.isZero() || now >= nextCallTime_) {
        nextCallTime_ = now + _dispatcher();
    }
}

/**
 * @brief Suspend dispatcher work until it will be resumed
 */
void Module::suspend()
{
#if defined(FREERTOS_USED)
    // Suspend by FreeRTOS function or set suspend flag for no RTOS work
    if (xHandle_)
        vTaskSuspend(NULL);
    else
        flags_ |= kSuspended;
#else
    // Set suspend flag for no RTOS work
    flags_ |= kSuspended;
#endif
}

/**
 * @brief Checks suspended state of module
 *
 * @return true if suspended, otherwise false
 */
bool Module::isSuspended() const
{
    bool result = false;
#if defined(FREERTOS_USED)
    if (xHandle_)
        result = eTaskGetState(xHandle_) == eSuspended;
    else
        result = (flags_ & kSuspended) == kSuspended;
#else
    result = (flags_ & kSuspended) == kSuspended;
#endif
    return result;
}

/**
 * @brief Causes the next dispatcher call to execute as soon as possible
 *      regardless of nextCallTime and delayTime
 */
void Module::resume()
{
    // Zeroing delay time causes calling of the virtual _dispatcher function
    nextCallTime_ = 0;
#if defined(FREERTOS_USED)
    // With FreeRTOS also notify or resume task if it in suspended state,
    // or reset suspend flag for no RTOS work
    if (xHandle_) {
        if (eTaskGetState(xHandle_) == eSuspended)
            vTaskResume(xHandle_);
        else
            xTaskNotifyGive(xHandle_);
    } else {
        flags_ &= ~kSuspended;
    }
#else
    // Reset suspend flag for no RTOS work
    flags_ &= ~kSuspended;
#endif
}

/**
 * @brief Set the availability property of the module
 *
 * @param value new state
 */
void Module::setAvailability(bool value)
{
    if (value)
        flags_ |= kAvailability;
    else
        flags_ &= ~kAvailability;
}

#if defined(FREERTOS_USED)
/**
 * @brief FreeRTOS ONLY. Task function for working with the dispatcher
 */
void Module::task(void *instance)
{
    // Task not needed without instance
    if (!instance)
        vTaskDelete(NULL);

    while (1) {
        Module* mod = static_cast<Module*>(instance);
        mod->dispatcher();
        int32_t delayMs = mod->delayTime().toMsec();
        // Wait for delay time or break waiting on notification
        if (delayMs != 0)
            ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(delayMs));
    }
}
#else
/**
 * @brief Task function make infinite loop with the dispatcher
 */
void Module::task()
{
    while (1) {
        dispatcher();
    }
}
#endif

/***************************** END OF FILE ************************************/
