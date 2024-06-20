/*******************************************************************************
 * @file    can.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   GD32 CAN bus driver.
 ******************************************************************************/

#include "gd32/can.h"

#include <cassert>

namespace gd32 {

static bool checkConfig(const gd32::CanConfig* config)
{
    assert(config != nullptr);
    return config->can == CAN0 || config->can == CAN1;
}

bool Can::setConfig(const void* drvConfig)
{
    if (isOpen())
        return false;

    const gd32::CanConfig* config = static_cast<const gd32::CanConfig*>(drvConfig);
    if (!checkConfig(config)) {
        return false;
    }
    config_ = *config;
    return true;
}

bool Can::open()
{
    if (isOpen())
        return false;

    if (!checkConfig(&config_)) {
        return false;
    }

    rcu_periph_enum canClock;
    if (config_.can == CAN0) {
        canClock = RCU_CAN0;
    } else {
        canClock = RCU_CAN1;
    }
    rcu_periph_clock_enable(canClock);

    initGpioPeriph(&config_.tx);
    initGpioPeriph(&config_.rx);

    can_parameter_struct canParam;
    can_struct_para_init(CAN_INIT_STRUCT, &canParam);
    canParam.time_triggered = DISABLE;
    canParam.auto_bus_off_recovery = ENABLE;
    canParam.auto_wake_up = DISABLE;
    canParam.auto_retrans = ENABLE;
    canParam.rec_fifo_overwrite = DISABLE;
    canParam.trans_fifo_order = DISABLE;
    canParam.working_mode = CAN_NORMAL_MODE;
    canParam.resync_jump_width = CAN_BT_SJW_1TQ;
    canParam.time_segment_1 = CAN_BT_BS1_13TQ;
    canParam.time_segment_2 = CAN_BT_BS2_2TQ;
    canParam.prescaler = 21; // 42 MHz APB1 and 125 kBit/s

    can_deinit(config_.can);
    can_init(config_.can, &canParam);

    can_filter_parameter_struct canFilter;
    can_struct_para_init(CAN_FILTER_STRUCT, &canFilter);
    canFilter.filter_number=0;
    canFilter.filter_mode = CAN_FILTERMODE_MASK;
    canFilter.filter_bits = CAN_FILTERBITS_32BIT;
    canFilter.filter_list_high = 0x0000;
    canFilter.filter_list_low = 0x0000;
    canFilter.filter_mask_high = 0x0000;
    canFilter.filter_mask_low = 0x0000;
    canFilter.filter_fifo_number = CAN_FIFO0;
    canFilter.filter_enable = ENABLE;
    can_filter_init(&canFilter);

    setOpened(true);
    return true;
}

void Can::close()
{
    if (isOpen()) {
        can_deinit(config_.can);
        deinitGpioPeriph(&config_.tx);
        deinitGpioPeriph(&config_.rx);
        setOpened(false);
    }
}

int32_t Can::write_(const void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || len == 0)
        return -1;

    const uint8_t* bytes = static_cast<const uint8_t*>(buf);
    const int32_t addr = getAddr();
    const uint8_t length = len > 8 ? 8 : len;

    can_trasnmit_message_struct msg;
    if (addr <= CAN_SFID_MASK) {
        msg.tx_sfid = addr;
        msg.tx_efid = 0;
        msg.tx_ff = CAN_FF_STANDARD;
    } else {
        msg.tx_sfid = 0;
        msg.tx_efid = addr;
        msg.tx_ff = CAN_FF_EXTENDED;
    }
    msg.tx_ft = CAN_FT_DATA;
    msg.tx_dlen = length;

    for (uint8_t i = 0; i < length; ++i) {
        msg.tx_data[i] = bytes[i];
    }
    const uint8_t txMailbox = can_message_transmit(config_.can, &msg);
    return txMailbox != CAN_NOMAILBOX ? length : -1;
}

int32_t Can::read_(void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || len == 0)
        return -1;

    return len;
}

bool Can::ioctl(uint32_t cmd, void* pValue)
{
    if (!isOpen())
        return false;

    switch (static_cast<IoctlCmd>(cmd)) {
    case kSetBaudrate:
        if (pValue != nullptr) {
            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

}; // namespace gd32

/***************************** END OF FILE ************************************/
