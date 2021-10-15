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
/* Exported functions --------------------------------------------------------*/

/**
 * @brief Construct a new Module object
 */
Module::Module()
    : _delayTime(Time())
    , _nextCallTime(Time())
#if defined(FREERTOS_USED)
    , xHandle(NULL)
#endif
{
}

#if defined(FREERTOS_USED)
/**
 * @brief FreeRTOS ONLY. Construct a new Module object with
 *      internal task initialization
 *
 * @param name human readable task name
 * @param stack task stack size
 * @param prior task priority
 */
Module::Module(const char* name, uint32_t stack, UBaseType_t prior)
    : _delayTime(Time())
    , _nextCallTime(Time())
{
    xTaskCreate(task, name, stack, this, prior, &xHandle);
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
 * @brief Returns the Time object to wait for the next call to this
 *      object's dispatcher
 *
 * @return const Time& new call delay time
 */
const Time& Module::delayTime() const
{
    return _delayTime;
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
 *
 * @return const Time& new call delay time
 */
const Time& Module::dispatcher()
{
    // Dispatcher called only if was zero delay or time has come
    if (_delayTime.isZero() || Time::now() >= _nextCallTime) {
        _delayTime = _dispatcher();    // Call actual dispatcher function
        _nextCallTime = Time::now() + _delayTime;
    }
    return _delayTime;
}

/**
 * @brief Causes the next dispatcher call to execute as soon as possible
 *      regardless of nextCallTime and delayTime
 */
void Module::requestDispatcher()
{
    // Zeroing delayTime causes calling of the virtual _dispatcher function
    _delayTime = Time();
#if defined(FREERTOS_USED)
    // With FreeRTOS also notify or resume task if it in suspended state
    if (xHandle) {
        if (eTaskGetState(xHandle) == eSuspended)
            vTaskResume(xHandle);
        else
            xTaskNotifyGive(xHandle);
    }
#endif
}

/* Private functions ---------------------------------------------------------*/

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
        Time delay = static_cast<Module*>(instance)->dispatcher();
        // Wait for delay time or break waiting on notification
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(delay.toMsec()));
    }
}
#endif

/***************************** END OF FILE ************************************/
