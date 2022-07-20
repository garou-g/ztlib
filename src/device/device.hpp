/*******************************************************************************
 * @file    device.hpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of device module.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEVICE_H
#define __DEVICE_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#include "module.hpp"

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief Device class. It used only by singleton instance.
 *      Performs all working operations and main loop processing.
 *
 *      It has static part for maintaining device modules instances and saving
 *      it state in retain memory and also dynamic part that is initialized
 *      every boot with hardware specific instances, such as on board sensors.
 */
class Device : public Module {
public:
    static void initInstance();
    static Device& getInstance();

    /* Device methods --------------------------------------------------------*/

    uint32_t versionCode() const;

protected:
    Device(uint32_t defDispathDelayMs = 10U);
    virtual ~Device() {} // Empty virtual destructor for inheritance fix

    void setVersionCode(uint32_t code);

    /* Pure virutal methods for successors -----------------------------------*/

    virtual void initReset() = 0;
    virtual void initPowerLoss() = 0;
    virtual void initWakeup() = 0;
    virtual void dispatch() = 0;

    virtual void prepareToSleep() = 0;

private:
    enum State {
        kInit,
        kInited,
    };

    Time _dispatcher() override;

    static Device* device_;

    State state_;
    uint32_t versionCode_;
    const uint32_t dispatchDelayMs_;
};

#endif /* __DEVICE_H */

/***************************** END OF FILE ************************************/
