/*******************************************************************************
 * @file    timer.h
 * @author  doctorbelka avdbor.junior@mail.ru
 * @brief   Header file of timer.
 ******************************************************************************/

#pragma once

#include "periph/timer.h"
#include "gd32/gd32_types.h"

namespace gd32 {

/// @brief Timer configuration data structure
struct TimerConfig {
    int32_t timer;
    uint16_t prescaler;
    uint32_t period;
    uint32_t outputVal = 0;
    TimerMode mode;
    GpioConfig pin;
};

/// @brief GD32 Timer peripheral driver
class Timer : public ::Timer {
public:
    Timer() = default;

    bool setConfig(const void* drvConfig) override;
    bool open() override;
    void close() override;

    bool ioctl(uint32_t cmd, void* pValue) override;

    void start() override;
    void stop() override;

    uint32_t captured() override;

private:
    uint32_t captured_ = 0;
    TimerConfig config_;
};

}; // namespace gd32

/***************************** END OF FILE ************************************/
