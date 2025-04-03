/*******************************************************************************
 * @file    gd32_types.h
 * @author  4lagest (zakirdoion@gmail.com)
 * @brief   Header file of ADC GD32 specific types.
 ******************************************************************************/

#pragma once

#include "periph/adc.h"
#include "gd32/gd32_types.h"

namespace gd32 {

#if defined(GD32F4XX_H)
enum class AdcPrescaler {
    Pclk2Div2 = ADC_ADCCK_PCLK2_DIV2,
    Pclk2Div4 = ADC_ADCCK_PCLK2_DIV4,
    Pclk2Div6 = ADC_ADCCK_PCLK2_DIV6,
    Pclk2Div8 = ADC_ADCCK_PCLK2_DIV8,
    HclkDiv5 = ADC_ADCCK_HCLK_DIV5,
    HclkDiv6 = ADC_ADCCK_HCLK_DIV6,
    HclkDiv10 = ADC_ADCCK_HCLK_DIV10,
    HclkDiv20 = ADC_ADCCK_HCLK_DIV20,
};
#endif

struct AdcConfig {
    uint32_t adc;
    uint16_t channel;
    uint32_t resolution;
    uint32_t sampleTime;
#if defined(GD32F4XX_H)
    AdcPrescaler prescaler;
#endif
    AdcMode mode;
    GpioConfig gpioConfig;
};

#ifndef GD32F1XX__H
    #define GET_RESOLUTION(RESOLUTION_MACRO) \
        ((RESOLUTION_MACRO == ADC_RESOLUTION_12B) ? 12 : \
         (RESOLUTION_MACRO == ADC_RESOLUTION_10B) ? 10 : \
         (RESOLUTION_MACRO == ADC_RESOLUTION_8B)  ? 8  : \
         (RESOLUTION_MACRO == ADC_RESOLUTION_6B)  ? 6  : 0)
#else
    #define GET_RESOLUTION(RESOLUTION_MACRO) 12
#endif

}; // namespace gd32

/***************************** END OF FILE ************************************/
