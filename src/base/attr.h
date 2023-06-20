/*******************************************************************************
 * @file    attr.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of some cross-platform attributes definition.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ATTR_H
#define __ATTR_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ESP_PLATFORM)
/* ESP Section ---------------------------------------------------------------*/

#include "esp_attr.h"

#define NOINIT_ATTR                 __NOINIT_ATTR
#define RETAIN_NOINIT_ATTR          RTC_NOINIT_ATTR

#elif defined(GD32_PLATFORM)
/* GD32 Section --------------------------------------------------------------*/

#define NOINIT_ATTR                 __attribute__ ((section (".noinit")))
#define RETAIN_NOINIT_ATTR          __attribute__ ((section (".noinit")))

#else
/* Unknown platform ----------------------------------------------------------*/

#define NOINIT_ATTR
#define RETAIN_NOINIT_ATTR

#endif

#ifdef __cplusplus
}
#endif

#endif /* __ATTR_H */

/***************************** END OF FILE ************************************/
