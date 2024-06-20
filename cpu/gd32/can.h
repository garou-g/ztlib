/*******************************************************************************
 * @file    can.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of GD32 CAN bus driver.
 ******************************************************************************/

#pragma once

#include "periph/can.h"
#include "gd32/gd32_types.h"

namespace gd32 {

struct CanConfig {
    int32_t can;
    uint32_t baudrate;
    GpioConfig tx;
    GpioConfig rx;
};

/// @brief GD32 CAN bus peripheral driver
class Can : public ::Can {
public:
    Can() = default;

    bool setConfig(const void* drvConfig) override;
    bool open() override;
    void close() override;

    bool ioctl(uint32_t cmd, void* pValue) override;

private:
    int32_t write_(const void* buf, uint32_t len) override;
    int32_t read_(void* buf, uint32_t len) override;

    CanConfig config_;
};

}; // namespace gd32

/***************************** END OF FILE ************************************/
