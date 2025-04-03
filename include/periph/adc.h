/*******************************************************************************
 * @file    adc.h
 * @author  4lagest (zakirodion@gmail.com)
 * @brief   Header file of adc peripheral driver.
 ******************************************************************************/
#pragma once

#include "basedrv.h"

enum class AdcMode {
    OneShoot,
    Continuous,
};

/// @brief ADC peripheral driver
class Adc : public BaseDrv {
public:
    enum IoctlCmd {
        kSetResolution,
        kSetSampleTime,
        kSetMode,
    };

    virtual void start() = 0;
    virtual bool isReady() = 0;
    virtual uint16_t value() = 0;
};
