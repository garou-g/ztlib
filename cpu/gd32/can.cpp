/*******************************************************************************
 * @file    can.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   GD32 CAN bus driver.
 ******************************************************************************/

#include "gd32/can.h"

#include <cassert>

namespace gd32 {

static Can* canInstances[2] = { nullptr };

static bool checkConfig(const gd32::CanConfig* config)
{
    assert(config != nullptr);
    return config->can == CAN0 || config->can == CAN1;
}

static void setHandler(uint32_t port, Can* arg)
{
    switch (port) {
    case CAN0: canInstances[0] = arg; break;
    case CAN1: canInstances[1] = arg; break;
    default: return;
    }
}

/**
 * @brief Enables clock of chosen can port
 *
 * @param port can port
 */
static void enableClock(uint32_t port)
{
    rcu_periph_enum portClk;
    switch (port) {
    case CAN0: portClk = RCU_CAN0; break;
#if defined(GD32F4XX_H) || defined(GD32F10X_CL) || defined(GD32F30X_CL)
    case CAN1: portClk = RCU_CAN1; break;
#endif
    default: return;
    }
    rcu_periph_clock_enable(portClk);
}

static void setIrq(uint32_t port, bool enable)
{
    IRQn_Type irqType, irqType2;
    switch (port) {
#if defined(GD32F4XX_H) || defined(GD32F10X_CL) || defined(GD32F30X_CL)
    case CAN0: irqType = CAN0_RX0_IRQn; irqType2 = CAN0_RX1_IRQn; break;
#else
    case CAN0: irqType = USBD_LP_CAN0_RX0_IRQn; irqType2 = CAN0_RX1_IRQn; break;
#endif
#if defined(GD32F4XX_H) || defined(GD32F10X_CL) || defined(GD32F30X_CL)
    case CAN1: irqType = CAN1_RX0_IRQn; irqType2 = CAN1_RX1_IRQn; break;
#endif
    default: return;
    }

    if (enable) {
        NVIC_SetPriority(irqType, 7);
        NVIC_EnableIRQ(irqType);
        NVIC_SetPriority(irqType2, 7);
        NVIC_EnableIRQ(irqType2);
    } else {
        NVIC_DisableIRQ(irqType);
        NVIC_DisableIRQ(irqType2);
    }
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

    initGpioPeriph(&config_.tx);
    initGpioPeriph(&config_.rx);

    enableClock(config_.can);

    can_parameter_struct canParam;
    can_struct_para_init(CAN_INIT_STRUCT, &canParam);
    canParam.time_triggered = DISABLE;
    canParam.auto_bus_off_recovery = ENABLE;
    canParam.auto_wake_up = DISABLE;
    canParam.auto_retrans = DISABLE;
    canParam.rec_fifo_overwrite = DISABLE;
    canParam.trans_fifo_order = DISABLE;
    canParam.working_mode = CAN_NORMAL_MODE;
    canParam.resync_jump_width = CAN_BT_SJW_1TQ;

    // For APB1 frequency 42 MHz
    switch (config_.baudrate) {
    case CanBaud::Baud50k:
        canParam.time_segment_1 = CAN_BT_BS1_12TQ;
        canParam.time_segment_2 = CAN_BT_BS2_2TQ;
        canParam.prescaler = 56;
        break;
    case CanBaud::Baud125k:
        canParam.time_segment_1 = CAN_BT_BS1_13TQ;
        canParam.time_segment_2 = CAN_BT_BS2_2TQ;
        canParam.prescaler = 21;
        break;
    case CanBaud::Baud250k:
        canParam.time_segment_1 = CAN_BT_BS1_11TQ;
        canParam.time_segment_2 = CAN_BT_BS2_2TQ;
        canParam.prescaler = 12;
        break;
    case CanBaud::Baud500k:
        canParam.time_segment_1 = CAN_BT_BS1_11TQ;
        canParam.time_segment_2 = CAN_BT_BS2_2TQ;
        canParam.prescaler = 6;
        break;
    case CanBaud::Baud1M:
        canParam.time_segment_1 = CAN_BT_BS1_11TQ;
        canParam.time_segment_2 = CAN_BT_BS2_2TQ;
        canParam.prescaler = 3;
    default:
        break;
    }

    can_deinit(config_.can);
    can_init(config_.can, &canParam);
    can_interrupt_enable(config_.can, CAN_INT_RFNE0);

    can_filter_parameter_struct canFilter;
    can_struct_para_init(CAN_FILTER_STRUCT, &canFilter);
    canFilter.filter_number = 0;
    canFilter.filter_mode = CAN_FILTERMODE_MASK;
    canFilter.filter_bits = CAN_FILTERBITS_32BIT;
    canFilter.filter_list_high = config_.filterId >> 13;
    canFilter.filter_list_low = config_.filterId << 3;
    canFilter.filter_mask_high = config_.filterMask >> 13;
    canFilter.filter_mask_low = config_.filterMask << 3;
    canFilter.filter_fifo_number = CAN_FIFO0;
    canFilter.filter_enable = ENABLE;
    can_filter_init(&canFilter);

    setHandler(config_.can, this);
    setIrq(config_.can, true);
    setOpened(true);
    return true;
}

void Can::close()
{
    if (isOpen()) {
        setIrq(config_.can, false);
        setHandler(config_.can, nullptr);
        can_deinit(config_.can);
        deinitGpioPeriph(&config_.tx);
        deinitGpioPeriph(&config_.rx);
        setOpened(false);
    }
}

/**
 * @brief Writes CAN data message to driver
 *
 * @param msg CAN message structure
 * @return int32_t >0 if success, -1 on error
 */
int32_t Can::write(const CanMsg& msg)
{
    if (!isOpen())
        return -1;

    can_trasnmit_message_struct canMsg;
    if (msg.id <= CAN_SFID_MASK) {
        canMsg.tx_sfid = msg.id;
        canMsg.tx_efid = 0;
        canMsg.tx_ff = CAN_FF_STANDARD;
    } else {
        canMsg.tx_sfid = 0;
        canMsg.tx_efid = msg.id;
        canMsg.tx_ff = CAN_FF_EXTENDED;
    }
    canMsg.tx_ft = CAN_FT_DATA;
    canMsg.tx_dlen = msg.size;

    for (uint8_t i = 0; i < msg.size; ++i) {
        canMsg.tx_data[i] = msg.data[i];
    }
    const uint8_t txMailbox = can_message_transmit(config_.can, &canMsg);
    return txMailbox != CAN_NOMAILBOX ? 1 : -1;
}

/**
 * @brief Reads CAN data message from driver
 *
 * @param msg CAN message structure
 * @return int32_t >0 if success, -1 on error
 */
int32_t Can::read(CanMsg& msg)
{
    if (!isOpen())
        return -1;

    if (!rxQueue_.empty()) {
        msg = rxQueue_.front();
        rxQueue_.pop();
        return 1;
    } else {
        return -1;
    }
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

void Can::irqHandler(Can* can, uint8_t fifo)
{
    // Check instance pointer
    if (!can)
        return;

    CanMsg msg;
    can_receive_message_struct canMsg;

    // Get messages count in FIFO and read it all
    const uint8_t count = can_receive_message_length_get(can->config_.can, fifo);
    for (uint8_t i = 0; i < count; ++i) {
        can_message_receive(can->config_.can, fifo, &canMsg);
        if (canMsg.rx_ff == CAN_FF_STANDARD) {
            msg.id = canMsg.rx_sfid;
        } else {
            msg.id = canMsg.rx_efid;
        }
        msg.size = canMsg.rx_dlen;
        for (uint8_t i = 0; i < msg.size; ++i) {
            msg.data[i] = canMsg.rx_data[i];
        }
        can->rxQueue_.push(msg);
    }
}

extern "C" void CAN0_RX0_IRQHandler(void)
{
    Can::irqHandler(canInstances[0], CAN_FIFO0);
}

extern "C" void CAN0_RX1_IRQHandler(void)
{
    Can::irqHandler(canInstances[0], CAN_FIFO1);
}

extern "C" void CAN1_RX0_IRQHandler(void)
{
    Can::irqHandler(canInstances[1], CAN_FIFO0);
}

extern "C" void CAN1_RX1_IRQHandler(void)
{
    Can::irqHandler(canInstances[1], CAN_FIFO1);
}

}; // namespace gd32

/***************************** END OF FILE ************************************/
