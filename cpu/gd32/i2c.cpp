/*******************************************************************************
 * @file    i2c.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   GD32 I2C bus driver.
 ******************************************************************************/

#include "i2c.h"
#include "gd32_types.h"

#include <cassert>

enum class State {
    Start,
    SendAddress,
    TransmitReg,
    TransmitData,
    Stop,
};

static bool waitForFlagSet(int32_t i2c, i2c_flag_enum flag);

bool I2c::open(const void* drvConfig)
{
    if (isOpen())
        return false;

    assert(drvConfig != nullptr);
    const gd32::I2cConfig* config
        = static_cast<const gd32::I2cConfig*>(drvConfig);
    if (config->i2c == 0 || !(config->i2c == I2C0 || config->i2c == I2C1))
        return false;

    i2c_ = config->i2c;

    rcu_periph_enum i2cClock;
    if (i2c_ == I2C0) {
        i2cClock = RCU_I2C0;
    } else {
        i2cClock = RCU_I2C1;
    }
    rcu_periph_clock_enable(i2cClock);

    initGpioPeriph(&config->scl);
    initGpioPeriph(&config->sda);

    i2c_clock_config(i2c_, config->speed, I2C_DTCY_2);
    i2c_mode_addr_config(i2c_, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0);
    i2c_enable(i2c_);
    i2c_ack_config(i2c_, I2C_ACK_ENABLE);
    setOpened(true);

    return true;
}

void I2c::close()
{
    if (isOpen()) {
        i2c_disable(i2c_);
        i2c_deinit(i2c_);
        setOpened(false);
        i2c_ = -1;
    }
}

int32_t I2c::write_(const void* buf, uint32_t len)
{
    assert(buf != nullptr);

    // Take address from parent first if exists
    int32_t addr = getAddr() >= 0 ? (getAddr() << 1) : -1;
    if (!isOpen() || addr < 0 || i2c_flag_get(i2c_, I2C_FLAG_I2CBSY))
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
            i2c_start_on_bus(i2c_);
            if (waitForFlagSet(i2c_, I2C_FLAG_SBSEND)) {
                state = State::SendAddress;
            } else {
                cycle = false;
            }
            break;

        case State::SendAddress:
            i2c_master_addressing(i2c_, addr, I2C_TRANSMITTER);
            if (waitForFlagSet(i2c_, I2C_FLAG_ADDSEND)) {
                i2c_flag_clear(i2c_, I2C_FLAG_ADDSEND);
                state = reg >= 0 ? State::TransmitReg : State::TransmitData;
            } else {
                cycle = false;
            }
            break;

        case State::TransmitReg:
            i2c_data_transmit(i2c_, reg);
            state = State::TransmitData;
            break;

        case State::TransmitData:
            if (cnt < len) {
                i2c_data_transmit(i2c_, bytes[cnt++]);
                if (!waitForFlagSet(i2c_, I2C_FLAG_TBE)) {
                    cycle = false;
                }
            } else {
                state = State::Stop;
            }
            break;

        case State::Stop:
            i2c_stop_on_bus(i2c_);
            while (I2C_CTL0(i2c_) & 0x0200);
            res = len;
            cycle = false;
            break;
        }
    }

    return res;
}

int32_t I2c::read_(void* buf, uint32_t len)
{
    assert(buf != nullptr);

    // Take address from parent first if exists
    int32_t addr = getAddr() >= 0 ? (getAddr() << 1) : -1;
    if (!isOpen() || addr < 0 || len == 0 || i2c_flag_get(i2c_, I2C_FLAG_I2CBSY))
        return -1;

    uint8_t* bytes = static_cast<uint8_t*>(buf);
    const int32_t reg = getReg();
    bool cycle = true;
    State state = State::Start;
    int32_t res = -1;
    uint32_t cnt = 0;

    if (len == 2) {
        i2c_ackpos_config(i2c_, I2C_ACKPOS_NEXT);
    }

    while (cycle) {
        switch (state) {
        default:
        case State::Start:
            i2c_start_on_bus(i2c_);
            if (waitForFlagSet(i2c_, I2C_FLAG_SBSEND)) {
                state = State::SendAddress;
            } else {
                cycle = false;
            }
            break;

        case State::SendAddress:
            if (reg >= 0) {
                i2c_master_addressing(i2c_, addr, I2C_TRANSMITTER);
                if (waitForFlagSet(i2c_, I2C_FLAG_ADDSEND)) {
                    i2c_flag_clear(i2c_, I2C_FLAG_ADDSEND);
                    state = State::TransmitReg;
                } else {
                    cycle = false;
                }
            } else {
                i2c_master_addressing(i2c_, addr, I2C_RECEIVER);
                if (len < 3) {
                    i2c_ack_config(i2c_, I2C_ACK_DISABLE);
                }

                if (waitForFlagSet(i2c_, I2C_FLAG_ADDSEND)) {
                    i2c_flag_clear(i2c_, I2C_FLAG_ADDSEND);
                    if (len == 1) {
                        i2c_stop_on_bus(i2c_);
                    }
                    state = State::TransmitData;
                } else {
                    cycle = false;
                }
            }
            break;

        case State::TransmitReg:
            i2c_data_transmit(i2c_, reg);
            if (!waitForFlagSet(i2c_, I2C_FLAG_BTC)) {
                cycle = false;
            } else {
                i2c_start_on_bus(i2c_);
                if (waitForFlagSet(i2c_, I2C_FLAG_SBSEND)) {
                    i2c_master_addressing(i2c_, addr, I2C_RECEIVER);
                    if (len < 3) {
                        i2c_ack_config(i2c_, I2C_ACK_DISABLE);
                    }

                    if (waitForFlagSet(i2c_, I2C_FLAG_ADDSEND)) {
                        i2c_flag_clear(i2c_, I2C_FLAG_ADDSEND);
                        if (len == 1) {
                            i2c_stop_on_bus(i2c_);
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
                if (waitForFlagSet(i2c_, I2C_FLAG_BTC)) {
                    i2c_ack_config(i2c_, I2C_ACK_DISABLE);
                } else {
                    cycle = false;
                    break;
                }
            }
            if (len >= 2 && cnt == len - 2) {
                if (waitForFlagSet(i2c_, I2C_FLAG_BTC)) {
                    i2c_stop_on_bus(i2c_);
                } else {
                    cycle = false;
                    break;
                }
            }

            if (cnt < len) {
                if (waitForFlagSet(i2c_, I2C_FLAG_RBNE)) {
                    bytes[cnt++] = i2c_data_receive(i2c_);
                } else {
                    cycle = false;
                }
            } else {
                state = State::Stop;
            }
            break;

        case State::Stop:
            while (I2C_CTL0(i2c_) & 0x0200);
            res = len;
            cycle = false;
            break;
        }
    }

    i2c_ack_config(i2c_, I2C_ACK_ENABLE);
    return res;
}

bool I2c::ioctl(uint32_t cmd, void* pValue)
{
    if (!isOpen())
        return false;

    switch (static_cast<IoctlCmd>(cmd)) {
    case kSetAddress:
        if (pValue != nullptr) {
            setAddr(*static_cast<int32_t*>(pValue));
            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

static bool waitForFlagSet(int32_t i2c, i2c_flag_enum flag)
{
    const uint32_t timeout = 5000;
    uint32_t time = 0;
    // Wait until flag bit is set
    while (!i2c_flag_get(i2c, flag) && time < timeout) {
        ++time;
    }
    return time < timeout;
}

/***************************** END OF FILE ************************************/
