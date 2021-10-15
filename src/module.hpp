/*******************************************************************************
 * @file    module.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of base module class.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MODULE_H
#define __MODULE_H

/* Includes ------------------------------------------------------------------*/
#include "time.hpp"

#if defined(FREERTOS_USED)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief Base class for designing device modules. Has dispatcher method with
 *      checking time delays between calls. Can use FreeRTOS features internally
 *      for dispatcher calls without the need for external control
 *      (global define FREERTOS_USED).
 */
class Module {
public:
    Module();
#if defined(FREERTOS_USED)
    Module(const char* name, uint32_t stack, UBaseType_t prior);
    void taskInit(const char* name, uint32_t stack, UBaseType_t prior);
#endif

    const Time& delayTime() const;
    const Time& nextCallTime() const;
    const Time& dispatcher();
    void requestDispatcher();

protected:
    /**
     * @brief Virtual dispatcher for overriding in successor classes. Returns
     *      the Time object to wait for the next call to this object's
     *      dispatcher or zero if the next call should be immediately
     *
     * @return Time new call delay time or zero value
     */
    virtual Time _dispatcher() = 0;

private:
    Time _delayTime;
    Time _nextCallTime;

#if defined(FREERTOS_USED)
    TaskHandle_t xHandle;
    static void task(void* instance);
#endif
};

#endif /* __MODULE_H */

/***************************** END OF FILE ************************************/
