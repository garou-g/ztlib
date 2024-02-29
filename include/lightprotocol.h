/*******************************************************************************
 * @file    lightprotocol.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Light serial exchange protocol.
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
class LightProt {
public:
    /// @brief Message service data - start of frame, length and crc
    static constexpr size_t kServiceSize = 5;
    /// @brief Message maximum size with service and data
    static constexpr size_t kMsgMaxSize = kServiceSize + MSG_SIZE;

    /// @brief Start of frame byte
    static constexpr uint8_t kSof = 0x01;
    /// @brief Acknowledge flag byte
    static constexpr uint8_t kAck = 0x06;
    /// @brief Not acknowledge flag byte
    static constexpr uint8_t kNak = 0x15;

    LightProt() = default;
    LightProt(const LightProt& other) = delete;
    LightProt(LightProt&& other) = delete;

    /// @brief Message data structure
    struct Msg {
        uint8_t cmd;
        uint8_t length;
        uint16_t crc;
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
     * @brief Writes data with protocol additional data
     *
     * @param cmd protocol command
     * @param data user data buffer
     * @param len data buffer length
     */
    void write(uint8_t cmd, const uint8_t* data, uint32_t len)
    {
        // Check input data
        if (data == nullptr || len == 0)
            return;

        // Check data length and make crc
        len = len < MSG_SIZE ? len : MSG_SIZE;
        const uint32_t msgLength = len + kServiceSize;
        const uint16_t crc = crc16(data, len);

        // Fill msg buffer and send
        uint8_t buf[msgLength];
        buf[0] = kSof;
        buf[1] = cmd;
        buf[2] = len;
        buf[msgLength - 2] = crc & 0xFF;
        buf[msgLength - 1] = (crc >> 8) & 0xFF;
        for (uint32_t i = 0; i < len; ++i) {
            buf[i + 3] = data[i];
        }

        // Write if delegate was set
        deleg_.write.call_if(buf, msgLength);
    }

    /// @brief Writes ACK flag that mean success reception of the last message
    void ack()
    {
        uint8_t byte = kAck;
        deleg_.write.call_if(&byte, 1);
    }

    /// @brief Writes NAK flag that mean errors in reception of the last message
    void nak()
    {
        uint8_t byte = kNak;
        deleg_.write.call_if(&byte, 1);
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
                switch (byte) {
                // Start of frame received - start message reception
                case kSof:
                    receiveTimeout_ = Time::now() + kReceiveTimeoutMs;
                    state_ = State::Cmd;
                    break;
                // ACK/NAK received - just call callback
                case kAck:
                case kNak:
                    deleg_.ack.call_if(byte);
                    break;
                // Nothing to do if other
                default:
                    break;
                }
                break;

            case State::Cmd:
                msg_.cmd = byte;
                state_ = State::Length;
                break;

            case State::Length:
                msg_.length = byte < MSG_SIZE ? byte : MSG_SIZE;
                step = 0;
                state_ = State::Data;
                break;

            case State::Data:
                msg_.data[step++] = byte;
                if (step >= msg_.length) {
                    step = 0;
                    state_ = State::Crc;
                }
                break;

            case State::Crc:
                if (step == 0) {
                    msg_.crc = byte & 0xFF;
                    step = 1;
                } else {
                    msg_.crc |= (byte & 0xFF) << 8;

                    // Check message checksum
                    uint16_t crc = crc16(msg_.data, msg_.length);
                    if (crc == msg_.crc) {
                        // Received success
                        ack();
                        deleg_.parse.call_if(msg_);
                    } else {
                        // CRC is wrong
                        nak();
                    }

                    // Message finished, reset timeout
                    receiveTimeout_ = 0;
                    state_ = State::Idle;
                }
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
        Start,
        Cmd,
        Length,
        Data,
        Crc,
    };

    State state_ = State::Idle;
    Time receiveTimeout_ = 0;
    Msg msg_ = {};
    Delegates deleg_;
};

/***************************** END OF FILE ************************************/
