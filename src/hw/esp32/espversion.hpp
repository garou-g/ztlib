/*******************************************************************************
 * @file    espversion.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of ESP32 version ADC driver.
 ******************************************************************************/

#pragma once

#include "version.hpp"

/**
 * @brief ADC processing class for version reading from resistors network
 */
class EspVersion : public Version {
public:
    EspVersion();

protected:
    HwVolt getHwValue() override;
    FwString getFwValue() override;
};

/***************************** END OF FILE ************************************/
