/*******************************************************************************
 * @file    espversion.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of ESP32 version ADC driver.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESP_VERSION_H
#define __ESP_VERSION_H

/* Includes ------------------------------------------------------------------*/
#include "version.hpp"

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

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

#endif /* __ESP_VERSION_H */

/***************************** END OF FILE ************************************/
