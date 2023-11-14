/*******************************************************************************
 * @file    drvmodule.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of base module with driver instance class.
 ******************************************************************************/

#pragma once

#include "module.hpp"

/// Macro to get a driver instance with a static cast for the required class
#define DRV(type) static_cast<type*>(drv())

/**
 * @brief Base class for designing device modules that uses drivers to access
 *      peripheral or other kind of dependences. For example sensor modules can
 *      have drivers with concrete sensor realization.
 */
class DrvModule : public Module {
public:
    DrvModule();
#if defined(FREERTOS_USED)
    DrvModule(const char* name, uint32_t stack, UBaseType_t prior);
#endif
    virtual ~DrvModule() = default;

    void setDriver(void* drvInst);

protected:
    void* drv() const;

private:
    void* drv_;
};

/***************************** END OF FILE ************************************/
