/*******************************************************************************
 * @file    drvmodule.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Base class for other modules with driver instance.
 ******************************************************************************/

#include "drvmodule.hpp"

/**
 * @brief Construct a new DrvModule object with only driver initialization.
 *      Unlike base Module constructor DrvModule by default is not available,
 *      until driver instance is set
 */
DrvModule::DrvModule()
    : drv_(nullptr)
{
    setAvailability(false);
}

#if defined(FREERTOS_USED)
/**
 * @brief FreeRTOS ONLY. Construct a new DrvModule object with
 *      internal task initialization. Also calls init inside because
 *      task starts right after init
 *
 * @param name human readable task name
 * @param stack task stack size
 * @param prior task priority
 */
DrvModule::DrvModule(const char* name, uint32_t stack, UBaseType_t prior)
    : Module(name, stack, prior)
    , drv_(nullptr)
{
    setAvailability(false);
}
#endif

/**
 * @brief Sets module driver instance
 *
 * @param drvInst driver instance
 */
void DrvModule::setDriver(void* drvInst)
{
    drv_ = drvInst;
    setAvailability(drv_ != nullptr);
}

/**
 * @brief Returns module driver instance
 *
 * @return void* driver instance pointer
 */
void* DrvModule::drv() const
{
    return drv_;
}

/***************************** END OF FILE ************************************/
