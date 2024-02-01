/*******************************************************************************
 * @file    drvmodule.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of base module with driver instance class.
 ******************************************************************************/

#pragma once

#include "module.h"

/**
 * @brief Base class for designing device modules that uses drivers to access
 *      peripheral or other kind of dependences. For example sensor modules can
 *      have drivers with concrete sensor realization.
 */
template<typename Drv>
class DrvModule : public Module {
public:
    /**
     * @brief Construct a new DrvModule object with only driver initialization.
     *      Unlike base Module constructor DrvModule by default is not available,
     *      until driver instance is set
     */
    DrvModule()
    : drv_(nullptr) {
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
        , drv_(nullptr) {
            setAvailability(false);
        }
#endif
    virtual ~DrvModule() = default;

    void setDriver(Drv* drvInst) {
        drv_ = drvInst;
        setAvailability(drv_ != nullptr);
    }

protected:
    Drv* drv() const {
        return drv_;
    }

private:
    Drv* drv_;
};

/***************************** END OF FILE ************************************/
