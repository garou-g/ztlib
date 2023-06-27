/*******************************************************************************
 * @file    gd32i2c.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   GD32 I2C bus driver.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "gd32i2c.hpp"

#include <cassert>

/**
 * @brief Default empty constructor
 */
Gd32I2c::Gd32I2c()
    : I2c()
    , i2c(-1)
    , addr(-1)
{
}

bool Gd32I2c::open(const void* drvConfig)
{
    if (isOpen())
        return false;

    assert(drvConfig != nullptr);
    const Gd32I2c::Config* config
        = static_cast<const Gd32I2c::Config*>(drvConfig);

    if (config->i2c == 0 || !(config->i2c == I2C0 || config->i2c == I2C1))
        return false;
    i2c = config->i2c;

    rcu_periph_enum i2cClock;
    switch (i2c) {
    default:
    case I2C0: i2cClock = RCU_I2C0; break;
    case I2C1: i2cClock = RCU_I2C1; break;
    }
    rcu_periph_clock_enable(i2cClock);
    rcu_periph_clock_enable(config->scl.clock);
    rcu_periph_clock_enable(config->sda.clock);

    gpio_init(config->scl.port, config->scl.mode, GPIO_OSPEED_50MHZ, config->scl.pin);
    gpio_init(config->sda.port, config->sda.mode, GPIO_OSPEED_50MHZ, config->sda.pin);

    i2c_clock_config(i2c, config->speed, I2C_DTCY_2);
    i2c_mode_addr_config(i2c, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0);
    i2c_enable(i2c);
    i2c_ack_config(i2c, I2C_ACK_ENABLE);
    setOpened(true);

    return true;
}

void Gd32I2c::close()
{
    if (isOpen()) {
        i2c_disable(i2c);
        i2c_deinit(i2c);
        setOpened(false);
        i2c = 0;
        addr = -1;
    }
}

int32_t Gd32I2c::write(const void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || addr < 0 || i2c_flag_get(i2c, I2C_FLAG_I2CBSY))
        return -1;

    const uint8_t* bytes = static_cast<const uint8_t*>(buf);
    const int32_t reg = getReg();
    bool cycle = true;
    State state = State::Start;
    int32_t res = -1;
    uint32_t cnt = 0;

    while (cycle) {
        switch (state) {
        default:
        case State::Start:
            i2c_start_on_bus(i2c);
            if (waitForFlagSet(I2C_FLAG_SBSEND)) {
                state = State::SendAddress;
            } else {
                cycle = false;
            }
            break;

        case State::SendAddress:
            i2c_master_addressing(i2c, addr, I2C_TRANSMITTER);
            if (waitForFlagSet(I2C_FLAG_ADDSEND)) {
                i2c_flag_clear(i2c, I2C_FLAG_ADDSEND);
                state = reg >= 0 ? State::TransmitReg : State::TransmitData;
            } else {
                cycle = false;
            }
            break;

        case State::TransmitReg:
            i2c_data_transmit(i2c, reg);
            state = State::TransmitData;
            break;

        case State::TransmitData:
            if (cnt < len) {
                i2c_data_transmit(i2c, bytes[cnt++]);
                if (!waitForFlagSet(I2C_FLAG_TBE)) {
                    cycle = false;
                }
            } else {
                state = State::Stop;
            }
            break;

        case State::Stop:
            i2c_stop_on_bus(i2c);
            while (I2C_CTL0(i2c) & 0x0200);
            res = len;
            cycle = false;
            break;
        }
    }

    return res;
}

int32_t Gd32I2c::read(void* buf, uint32_t len)
{
    assert(buf != nullptr);

    if (!isOpen() || addr < 0 || len == 0 || i2c_flag_get(i2c, I2C_FLAG_I2CBSY))
        return -1;

    uint8_t* bytes = static_cast<uint8_t*>(buf);
    const int32_t reg = getReg();
    bool cycle = true;
    State state = State::Start;
    int32_t res = -1;
    uint32_t cnt = 0;

    if (len == 2) {
        i2c_ackpos_config(i2c, I2C_ACKPOS_NEXT);
    }

    while (cycle) {
        switch (state) {
        default:
        case State::Start:
            i2c_start_on_bus(i2c);
            if (waitForFlagSet(I2C_FLAG_SBSEND)) {
                state = State::SendAddress;
            } else {
                cycle = false;
            }
            break;

        case State::SendAddress:
            if (reg >= 0) {
                i2c_master_addressing(i2c, addr, I2C_TRANSMITTER);
                if (waitForFlagSet(I2C_FLAG_ADDSEND)) {
                    i2c_flag_clear(i2c, I2C_FLAG_ADDSEND);
                    state = State::TransmitReg;
                } else {
                    cycle = false;
                }
            } else {
                i2c_master_addressing(i2c, addr, I2C_RECEIVER);
                if (len < 3) {
                    i2c_ack_config(i2c, I2C_ACK_DISABLE);
                }

                if (waitForFlagSet(I2C_FLAG_ADDSEND)) {
                    i2c_flag_clear(i2c, I2C_FLAG_ADDSEND);
                    if (len == 1) {
                        i2c_stop_on_bus(i2c);
                    }
                    state = State::TransmitData;
                } else {
                    cycle = false;
                }
            }
            break;

        case State::TransmitReg:
            i2c_data_transmit(i2c, reg);
            if (!waitForFlagSet(I2C_FLAG_BTC)) {
                cycle = false;
            } else {
                i2c_start_on_bus(i2c);
                if (waitForFlagSet(I2C_FLAG_SBSEND)) {
                    i2c_master_addressing(i2c, addr, I2C_RECEIVER);
                    if (len < 3) {
                        i2c_ack_config(i2c, I2C_ACK_DISABLE);
                    }

                    if (waitForFlagSet(I2C_FLAG_ADDSEND)) {
                        i2c_flag_clear(i2c, I2C_FLAG_ADDSEND);
                        if (len == 1) {
                            i2c_stop_on_bus(i2c);
                        }
                        state = State::TransmitData;
                    } else {
                        cycle = false;
                    }
                } else {
                    cycle = false;
                }
            }
            state = State::TransmitData;
            break;

        case State::TransmitData:
            if (len >= 3 && cnt == len - 3) {
                if (waitForFlagSet(I2C_FLAG_BTC)) {
                    i2c_ack_config(i2c, I2C_ACK_DISABLE);
                } else {
                    cycle = false;
                    break;
                }
            }
            if (len >= 2 && cnt == len - 2) {
                if (waitForFlagSet(I2C_FLAG_BTC)) {
                    i2c_stop_on_bus(i2c);
                } else {
                    cycle = false;
                    break;
                }
            }

            if (cnt < len) {
                if (waitForFlagSet(I2C_FLAG_RBNE)) {
                    bytes[cnt++] = i2c_data_receive(i2c);
                } else {
                    cycle = false;
                }
            } else {
                state = State::Stop;
            }
            break;

        case State::Stop:
            while (I2C_CTL0(i2c) & 0x0200);
            res = len;
            cycle = false;
            break;
        }
    }

    i2c_ack_config(i2c, I2C_ACK_ENABLE);
    return res;
}

bool Gd32I2c::ioctl(uint32_t cmd, void* pValue)
{
    if (!isOpen())
        return false;

    switch (static_cast<IoctlCmd>(cmd))
    {
    case kSetAddress:
        if (pValue != nullptr) {
            addr = *static_cast<int32_t*>(pValue) << 1;
            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

bool Gd32I2c::waitForFlagSet(i2c_flag_enum flag) const
{
    uint32_t time = 0;
    // Wait until flag bit is set
    while (!i2c_flag_get(i2c, flag) && time < timeout) {
        ++time;
    }
    return time < timeout;
}

/***************************** END OF FILE ************************************/
