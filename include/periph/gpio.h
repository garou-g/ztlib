/*******************************************************************************
 * @file    gpio.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of abstract gpio driver.
 ******************************************************************************/

#pragma once

#include "basedrv.h"

/// @brief Basic gpio driver config structure
struct GpioConfig {
    int32_t port;
    int32_t pin;
};

/// @brief Abstract gpio driver
class Gpio : public BaseDrv {
public:
    /**
     * @brief Default constructor
     */
    Gpio() = default;

    /**
     * @brief Sets configured gpio to high level
     */
    virtual void set() = 0;

    /**
     * @brief Sets configured gpio to low level
     */
    virtual void reset() = 0;

    /**
     * @brief Sets configured gpio to new state
     *
     * @param state sets to high if true otherwise sets to low
     */
    void setState(bool state) { if (state) set(); else reset(); }

    /**
     * @brief Gets current state of gpio
     *
     * @return true if gpio set otherwise false
     */
    virtual bool get() const = 0;
};

/***************************** END OF FILE ************************************/
