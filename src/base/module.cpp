/*******************************************************************************
 * @file    module.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Base class for other modules with time and event functions.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "module.hpp"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

const int Module::kSuspended = 0x1;
const int Module::kAvailability = 0x2;
const int Module::kInited = 0x55AA0000;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Construct a new Module object
 */
Module::Module()
#if defined(FREERTOS_USED)
    : xHandle(NULL)
#endif
{
    if (!isInited()) {
        _flags = kInited | kAvailability;
        _nextCallTime = 0;
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
    : xHandle(NULL)
{
    if (!isInited()) {
        _flags = kInited | kAvailability;
        _nextCallTime = 0;
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
    if (xHandle == NULL)
        xTaskCreate(task, name, stack, this, prior, &xHandle);
}
#endif

/**
 * @brief Performs reset module to default state
 */
void Module::reset()
{
    _flags = kInited | kAvailability;
    _nextCallTime = 0;
}

/**
 * @brief Returns init state of module. Can be used for prevent reinit when
 *      used in retain memory and device restarts
 *
 * @return true if module was already inited otherwise false
 */
bool Module::isInited() const
{
    return (_flags & kInited) == kInited;
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
    return (_flags & kAvailability) == kAvailability;
}

/**
 * @brief Returns the Time object to wait for the next call to this
 *      object's dispatcher
 *
 * @return const Time new call delay time
 */
const Time Module::delayTime() const
{
    if (_nextCallTime.isZero())
        return 0;
    else {
        Time delta = _nextCallTime - Time::now();
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
    return _nextCallTime;
}

/**
 * @brief Module dispatcher function. Calls the virtual dispatch function
 *      to actually work in the successor classes. Returns the Time object
 *      to wait for the next call to this object's dispatcher
 */
void Module::dispatcher()
{
    // If suspended - no processing, just return
    if (isSuspended())
        return;

    // Dispatcher called only if was zero delay or time has come
    Time now = Time::now();
    if (_nextCallTime.isZero() || now >= _nextCallTime) {
        _nextCallTime = now + _dispatcher();
    }
}

/**
 * @brief Suspend dispatcher work until it will be resumed
 */
void Module::suspend()
{
#if defined(FREERTOS_USED)
    // Suspend by FreeRTOS function or set suspend flag for no RTOS work
    if (xHandle)
        vTaskSuspend(NULL);
    else
        _flags |= kSuspended;
#else
    // Set suspend flag for no RTOS work
    _flags |= kSuspended;
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
    if (xHandle)
        result = eTaskGetState(xHandle) == eSuspended;
    else
        result = (_flags & kSuspended) == kSuspended;
#else
    result = (_flags & kSuspended) == kSuspended;
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
    _nextCallTime = 0;
#if defined(FREERTOS_USED)
    // With FreeRTOS also notify or resume task if it in suspended state,
    // or reset suspend flag for no RTOS work
    if (xHandle) {
        if (eTaskGetState(xHandle) == eSuspended)
            vTaskResume(xHandle);
        else
            xTaskNotifyGive(xHandle);
    } else {
        _flags &= ~kSuspended;
    }
#else
    // Reset suspend flag for no RTOS work
    _flags &= ~kSuspended;
#endif
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Set the availability property of the module
 *
 * @param value new state
 */
void Module::setAvailability(bool value)
{
    if (value)
        _flags |= kAvailability;
    else
        _flags &= ~kAvailability;
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
#endif

/***************************** END OF FILE ************************************/
