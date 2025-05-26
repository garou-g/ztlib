/*******************************************************************************
 * @file    lightprotocol2.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Second variant of light serial exchange protocol.
 ******************************************************************************/

#pragma once

#include "timing.h"
#include "crc.h"

#include "etl/delegate.h"

/**
 * @brief Simple protocol realization for serial interfaces
 *
 * @tparam MSG_SIZE data buffer maximum size
 */
template <const size_t MSG_SIZE>
class LightProt2 {
public:
    /// @brief Message service data - start flags, length and crc
    static constexpr size_t kServiceSize = 6;
    /// @brief Message maximum size with service and data
    static constexpr size_t kMsgMaxSize = kServiceSize + MSG_SIZE;

    /// @brief Start of frame first byte
    static constexpr uint8_t kMsgFlag1 = 0x17;
    /// @brief Start of frame second byte
    static constexpr uint8_t kMsgFlag2 = 0xAA;
    /// @brief Acknowledge flag byte
    static constexpr uint8_t kAck = 0x06;
    /// @brief Not acknowledge flag byte
    static constexpr uint8_t kNak = 0x15;

    LightProt2() = default;
    LightProt2(const LightProt2& other) = delete;
    LightProt2(LightProt2&& other) = delete;

    /// @brief Message data structure
    struct Msg {
        uint16_t length;
        uint8_t data[MSG_SIZE];
    };

    using ParseDelegate = etl::delegate<void(const Msg&)>;
    using AckDelegate = etl::delegate<void(uint8_t)>;
    using WriteDelegate = etl::delegate<int32_t(const void*, uint32_t)>;

    /// @brief Delegates for process received messages and write output data
    struct Delegates {
        ParseDelegate parse;
        AckDelegate ack;
        WriteDelegate write;
    };

    /**
     * @brief Set the delegates for external interactions
     *
     * @param deleg delegates structure
     */
    void setDelegates(const Delegates& deleg)
    {
        deleg_ = deleg;
    }

    /**
     * @brief Set the auto ack/nak sending after message reception
     *
     * @param state new state of auto ack/nak sending
     */
    void setAutoAck(bool state)
    {
        autoAck_ = state;
    }

    /**
     * @brief Returns current state of auto ack/nak sending after message reception
     *
     * @return true if enabled, otherwise disabled
     */
    bool autoAck() const
    {
        return autoAck_;
    }

    /**
     * @brief Writes data with protocol additional data
     *
     * @param data user data buffer
     * @param len data buffer length
     */
    void write(const uint8_t* data, uint32_t len)
    {
        // Check input data
        if (data == nullptr || len == 0)
            return;

        // Check data length and make crc
        len = len < MSG_SIZE ? len : MSG_SIZE;
        const uint16_t crc = crc16(data, len);

        // Fill msg buffer and send
        uint8_t buf[len + kServiceSize];
        buf[0] = kMsgFlag1;
        buf[1] = kMsgFlag2;
        buf[2] = len;
        buf[3] = len >> 8;
        buf[sizeof(buf) - 2] = crc;
        buf[sizeof(buf) - 1] = crc >> 8;
        for (uint32_t i = 0; i < len; ++i) {
            buf[i + 4] = data[i];
        }

        // Write if delegate was set
        deleg_.write.call_if(buf, sizeof(buf));
    }

    /// @brief Writes ACK flag that mean success reception of the last message
    void ack()
    {
        const uint8_t buf[] = {
            kMsgFlag1,
            kAck,
        };
        deleg_.write.call_if(buf, sizeof(buf));
    }

    /// @brief Writes NAK flag that mean errors in reception of the last message
    void nak()
    {
        const uint8_t buf[] = {
            kMsgFlag1,
            kNak,
        };
        deleg_.write.call_if(buf, sizeof(buf));
    }

    /**
     * @brief Processing of reception loop. Needs to be called periodically
     *      from external user
     *
     * @param data raw received data buffer
     * @param len data buffer length
     */
    void process(const uint8_t* data, int32_t len)
    {
        static uint32_t step = 0; // For message internal counters

        // Check received data length
        const int32_t readed = len > kMsgMaxSize ? kMsgMaxSize : len;
        if (readed <= 0) {
            // Check message reception timeout and reset state if needed
            if (receiveTimeout_ != 0 && Time::isPast(receiveTimeout_)) {
                receiveTimeout_ = 0;
                state_ = State::Idle;
                nak(); // Inform that something goes wrong
            }
            return;
        }

        // Process each readed byte
        for (int32_t i = 0; i < readed; ++i) {
            const uint8_t byte = data[i];

            switch (state_) {
            case State::Idle:
                if (byte == kMsgFlag1) {
                    receiveTimeout_ = Time::now() + kReceiveTimeoutMs;
                    state_ = State::Header;
                }
                break;

            case State::Header:
                switch (byte) {
                // Second flag received - start message reception
                case kMsgFlag2:
                    state_ = State::Length1;
                    break;
                // ACK/NAK received - call callback
                case kAck:
                case kNak:
                    deleg_.ack.call_if(byte);
                    receiveTimeout_ = 0;
                    state_ = State::Idle;
                    break;
                // Reset if other
                default:
                    receiveTimeout_ = 0;
                    state_ = State::Idle;
                    break;
                }
                break;

            case State::Length1:
                if (byte != 0) {
                    msg_.length = byte;
                    state_ = State::Length2;
                } else {
                    receiveTimeout_ = 0;
                    state_ = State::Idle;
                    break;
                }
                break;

            case State::Length2:
                msg_.length |= (byte << 8);
                if (msg_.length > MSG_SIZE) {
                    msg_.length = MSG_SIZE;
                }
                step = 0;
                state_ = State::Data;
                break;

            case State::Data:
                msg_.data[step++] = byte;
                if (step >= msg_.length) {
                    state_ = State::Crc1;
                }
                break;

            case State::Crc1:
                step = byte;
                state_ = State::Crc2;
                break;

            case State::Crc2:
                step |= (byte << 8);
                if (step == crc16(msg_.data, msg_.length)) {
                    // Received success
                    if (autoAck_)
                        ack();
                    deleg_.parse.call_if(msg_);
                } else {
                    // CRC is wrong
                    if (autoAck_)
                        nak();
                }

                // Message finished, reset timeout
                receiveTimeout_ = 0;
                state_ = State::Idle;
                break;

            default:
                break;
            }
        }
    }

private:
    /// @brief Default message receive timeout
    static constexpr uint32_t kReceiveTimeoutMs = 100;

    /// @brief Internal states enumeration
    enum class State {
        Idle,
        Header,
        Length1,
        Length2,
        Data,
        Crc1,
        Crc2,
    };

    State state_ = State::Idle;
    Time receiveTimeout_ = 0;
    Msg msg_ = {};
    Delegates deleg_;
    bool autoAck_ = true;
};

/***************************** END OF FILE ************************************/
