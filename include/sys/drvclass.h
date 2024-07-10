/*******************************************************************************
 * @file    drvclass.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of class with driver instance.
 ******************************************************************************/

#pragma once

#include <cstdint>
#include <cassert>

/**
 * @brief Base class for designing device classes that uses drivers to access
 *      peripheral or other kind of dependences. For example sensor modules can
 *      have drivers with concrete sensor realization.
 */
template<typename Drv>
class DrvClass {
public:
    /// @brief Construct a new DrvClass object with only driver initialization
    DrvClass() = default;

    /// @brief Default destructor
    virtual ~DrvClass() = default;

    /**
     * @brief Sets the driver instance
     *
     * @param drv driver instance
     */
    void setDriver(Drv* drv)
    {
        drv_ = drv;
    }

protected:
    /**
     * @brief Returns driver instance pointer
     *
     * @return Drv* driver instance pointer
     */
    Drv* drv() const
    {
        assert(drv_ != nullptr);
        return drv_;
    }

private:
    /// @brief Driver instance pointer
    Drv* drv_ = nullptr;
};

/***************************** END OF FILE ************************************/
