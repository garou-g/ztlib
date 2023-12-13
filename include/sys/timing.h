/*******************************************************************************
 * @file    timing.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of time measurement.
 ******************************************************************************/

#pragma once

#include <stdint.h>

/**
 * @brief Time manipulation class
 */
class Time {
public:
    static Time now();
    static bool isPast(const Time& start, const Time& delay);
    static bool isPast(const Time& end);
    static uint32_t getSystemTime();
    static bool setSystemTime(uint32_t utc, uint32_t maxDelta);

    static constexpr int32_t kSecondsInHour = 3600;
    static constexpr int32_t kMilisecondsInSecond = 1000;
    static constexpr int32_t kMilisecondsInHour = kSecondsInHour * kMilisecondsInSecond;

    Time();
    Time(int32_t ms);
    Time(int32_t h, int32_t s, int32_t ms);

    bool isZero() const;

    int32_t toSec() const;
    int32_t toMsec() const;
    int32_t toUTC() const;

    int32_t getHour() const;
    int32_t getSec() const;
    int32_t getMsec() const;
    static uint32_t getDeltaUTC();
    static void setDeltaUTC(uint32_t delta);

    Time& addSec(int32_t s);
    Time& addMsec(int32_t ms);

    const Time& operator=(const Time& c);
    Time& operator+=(const Time& c);
    Time& operator+=(int32_t c);
    Time& operator-=(const Time& c);
    Time& operator-=(int32_t c);

    const Time operator+(const Time& c) const;
    const Time operator+(int32_t c) const;
    const Time operator-(const Time& c) const;
    const Time operator-(int32_t c) const;

    bool operator==(const Time& c) const;
    bool operator!=(const Time& c) const;
    bool operator<(const Time& c) const;
    bool operator>(const Time& c) const;
    bool operator<=(const Time& c) const;
    bool operator>=(const Time& c) const;

private:
    void normalize(int32_t h, int32_t s, int32_t ms);

    static uint32_t deltaUTC;

    int32_t hour_;
    int16_t sec_;
    int16_t msec_;
};

/***************************** END OF FILE ************************************/
