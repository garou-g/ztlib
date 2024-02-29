/*******************************************************************************
 * @file    timer.h
 * @author  doctorbelka avdbor.junior@mail.ru
 * @brief   Header file of timer.
 ******************************************************************************/

#pragma once

#include "basedrv.h"

/// @brief Timer mode
enum TimerMode {
    General,
    Capture,
    Pwm
};

/// @brief Timer peripheral driver
class Timer : public BaseDrv {
public:
    enum IoctlCmd {
        kSetPrescale,   // Sets timer prescale
        kSetPeriod,     // Sets repetition value
        kSetOutputVal   // Sets output value in PWM mode
    };

    /// @brief Start timer
    virtual void start() = 0;
    /// @brief Stop timer
    virtual void stop() = 0;

    virtual uint32_t captured() = 0;
};

/***************************** END OF FILE ************************************/
