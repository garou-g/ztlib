/*******************************************************************************
 * @file    adc.h
 * @author  4lagest (zakirodion@gmail.com)
 * @brief   Header file of GD32 ADC driver.
 ******************************************************************************/

 #pragma once

 #include "gd32/adc_types.h"

namespace gd32 {

/// @brief GD32 ADC peripheral driver
class Adc final : public ::Adc {
public:
    Adc() = default;

    bool setConfig(const void* drvConfig) override;
    bool open() override;
    void close() override;
    bool ioctl(uint32_t cmd, void* pValue) override;

    void start() override;
    void stop() override;
    uint16_t value() override;
    bool isReady() override;

    static void irqHandler();

private:
    AdcConfig config_;
    uint16_t value_ = 0;
    bool ready_ = false;
    bool started_ = false;
};

}; // namespace gd32

/***************************** END OF FILE ************************************/
