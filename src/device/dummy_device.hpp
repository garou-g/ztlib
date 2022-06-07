/*******************************************************************************
 * @file    dummy_device.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Dummy device class.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DUMMY_DEVICE_H
#define __DUMMY_DEVICE_H

/* Includes ------------------------------------------------------------------*/
#include <cassert>

#include "device.hpp"

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief Dummy device used when no version detected
 */
class DummyDevice : public Device {
public:
    DummyDevice() { assert(false); }
    ~DummyDevice() {}

    void initReset() override {}
    void initPowerLoss() override {}
    void initWakeup() override {}
    void dispatch() override {}
    void prepareToSleep() override {}
};

#endif /* __DUMMY_DEVICE_H */

/***************************** END OF FILE ************************************/
