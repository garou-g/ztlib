/*******************************************************************************
 * @file    gd32gpio.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of GD32 mcu selection.
 ******************************************************************************/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GD32F10X_MD) || defined(GD32F10X_HD) || \
    defined(GD32F10X_XD) || defined(GD32F10X_CL)

#include "gd32f10x.h"

#elif defined(GD32F30X_HD) || defined(GD32F30X_XD) || defined(GD32F30X_CL)

#include "gd32f30x.h"

#elif defined(GD32F450) || defined(GD32F405) || defined(GD32F407) || \
    defined(GD32F470)  || defined(GD32F425) || defined(GD32F427)

#include "gd32f4xx.h"

#else

#error("Not defined GD32 CPU version!")

#endif

#ifdef __cplusplus
}
#endif

/***************************** END OF FILE ************************************/
