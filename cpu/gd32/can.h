/*******************************************************************************
 * @file    can.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of GD32 CAN bus driver.
 ******************************************************************************/

#pragma once

#include "periph/can.h"
#include "gd32/gd32_types.h"

#include "etl/queue.h"

namespace gd32 {

struct CanConfig {
    int32_t can;
    CanBaud baudrate;
    uint32_t filterId;
    uint32_t filterMask;
    GpioConfig tx;
    GpioConfig rx;
};

/// @brief GD32 CAN bus peripheral driver
class Can : public ::Can {
public:
    using CanQueue = etl::queue<CanMsg, 16, etl::memory_model::MEMORY_MODEL_MEDIUM>;

    Can() = default;

    bool setConfig(const void* drvConfig) override;
    bool open() override;
    void close() override;

    bool ioctl(uint32_t cmd, void* pValue) override;

    int32_t write(const CanMsg& msg) override;
    int32_t read(CanMsg& msg) override;

    static void irqHandler(Can* can, uint8_t fifo);

private:
    CanConfig config_;
    CanQueue rxQueue_;
};

}; // namespace gd32

/***************************** END OF FILE ************************************/
