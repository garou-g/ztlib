/*******************************************************************************
 * @file    espsystem.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of ESP32 global system functions.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESP_SYSTEM_H
#define __ESP_SYSTEM_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#include "system.hpp"

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief ESP system class with system-wide functions
 *      such as HW and FW versions, boot status and etc.
 */
class EspSystem final : public System {
public:
    EspSystem();
};

#endif /* __ESP_SYSTEM_H */

/***************************** END OF FILE ************************************/
