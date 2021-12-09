/*******************************************************************************
 * @file    systemproxy.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of system proxy class.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYSTEM_PROXY_H
#define __SYSTEM_PROXY_H

/* Includes ------------------------------------------------------------------*/
#include "system.hpp"

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief Proxy class for handle different version of System modules and
 *      construct it in order to current platform type
 */
class SystemProxy {
public:
    static System* initSystemByPlatform();
};

#endif /* __SYSTEM_PROXY_H */

/***************************** END OF FILE ************************************/
