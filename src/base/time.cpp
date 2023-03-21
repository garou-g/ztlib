/*******************************************************************************
 * @file    time.cpp
 * @author  garou (xgaroux@gmail.com)
 * @brief   Time measurement module.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <sys/time.h>

#include "time.hpp"
#include "attr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

const int32_t Time::kSecondsInHour = 3600;
const int32_t Time::kMilisecondsInSecond = 1000;
const int32_t Time::kMilisecondsInHour = 3600000;

/**
 * @brief UTC correction value is global for all Time instance.
 *      It needs to be placed in noinit section to prevent zeroing on restart.
 */
RETAIN_NOINIT_ATTR uint32_t Time::deltaUTC;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Returns current system time from device start
 *
 * @return Time current time object
 */
Time Time::now(void)
{
    struct timeval tvNow;
    gettimeofday(&tvNow, NULL);
    return Time(0, tvNow.tv_sec, tvNow.tv_usec / 1000);
}

/**
 * @brief Compares current time with start time mark and chosen delay.
 *
 * @param start start of time interval
 * @param delay delay time from start
 * @return true if current time exceeds delay otherwise false
 */
bool Time::isPast(const Time& start, const Time& delay)
{
    return Time::now() > (start + delay);
}

/**
 * @brief Compares current time with end time mark.
 *
 * @param end end time interval
 * @return true if current time exceeds end time otherwise false
 */
bool Time::isPast(const Time& end)
{
    return Time::now() > end;
}

/**
 * @brief Returns current system time in seconds with UTC correction.
 *      if deltaUTC is zero - returns only system time from start.
 *
 * @return uint32_t system time in seconds
 */
uint32_t Time::getSystemTime()
{
    return Time::now().toUTC();
}

/**
 * @brief
 *
 * @param utc actual UTC time in seconds
 * @param maxDelta acceptable delta in seconds on which current time can change
 *      maxDelta == 0 - don't check threshold and always do correction (server time)
 *      maxDelta != 0 - do correction if it below threshold
 * @return true if time correction was success otherwise false
 */
bool Time::setSystemTime(uint32_t utc, uint32_t maxDelta)
{
    // 1. Saves delta between UTC and current system time
    // 2. Then for restoring current system time use saved delta + current
    //      system time
    // NOTE: In this function don't change system time. Only deltaUTC

    uint32_t newDeltaUTC = utc - Time::now().toSec();
    uint32_t oldDeltaUTC = Time::getDeltaUTC();

    // Set correction if old UTC is zero (first time) or threshold not zero
    if (oldDeltaUTC != 0 && maxDelta != 0) {
        uint32_t correction = (newDeltaUTC >= oldDeltaUTC) ? newDeltaUTC - oldDeltaUTC : oldDeltaUTC - newDeltaUTC;
        if (correction > maxDelta)
            return false;
    }

    Time::setDeltaUTC(newDeltaUTC);
    return true;
}

/**
 * @brief Construct a new Time object without initialization
 */
Time::Time(void)
{
}

/**
 * @brief Construct a new Time object with milliseconds value
 *
 * @param ms milliseconds
 */
Time::Time(int32_t ms)
{
    normalize(0, 0, ms);
}

/**
 * @brief Construct a new Time object with given parameters
 *
 * @param h hours
 * @param s seconds
 * @param ms milliseconds
 */
Time::Time(int32_t h, int32_t s, int32_t ms)
{
    normalize(h, s, ms);
}

/**
 * @brief Checks if current object is zero Time - other words all
 *      fields is equal to zero
 *
 * @return true when all fields equal to zero, otherwise false
 */
bool Time::isZero() const
{
    return *this == 0;
}

/**
 * @brief Returns seconds representation of Time object
 *
 * @return int32_t seconds value
 */
int32_t Time::toSec(void) const
{
    return hour_ * kSecondsInHour + sec_;
}

/**
 * @brief Returns milliseconds representation of Time object
 *
 * @return int32_t milliseconds value
 */
int32_t Time::toMsec(void) const
{
    return hour_ * kMilisecondsInHour + sec_ * kMilisecondsInSecond + msec_;
}

/**
 * @brief Returns seconds representation of Time object
 *      with UTC correction
 *
 * @return int32_t seconds value with UTC correction
 */
int32_t Time::toUTC(void) const
{
    return hour_ * kSecondsInHour + sec_ + deltaUTC;
}

/**
 * @brief Time hours value
 *
 * @return int32_t hours
 */
int32_t Time::getHour(void) const
{
    return hour_;
}

/**
 * @brief Time seconds value
 *
 * @return int32_t seconds
 */
int32_t Time::getSec(void) const
{
    return sec_;
}

/**
 * @brief Time milliseconds value
 *
 * @return int32_t milliseconds
 */
int32_t Time::getMsec(void) const
{
    return msec_;
}

/**
 * @brief Global Time UTC correction value
 *
 * @return uint32_t UTC value
 */
uint32_t Time::getDeltaUTC(void)
{
    return deltaUTC;
}

/**
 * @brief Sets global Time UTC correction value
 *
 * @param delta new UTC value
 */
void Time::setDeltaUTC(uint32_t delta)
{
    deltaUTC = delta;
}

/**
 * @brief Adds seconds to current time and returns new value
 *
 * @param s seconds to add
 * @return Time& reference to the current object with added seconds
 */
Time& Time::addSec(int32_t s)
{
    normalize(hour_, sec_ + s, msec_);
    return *this;
}

/**
 * @brief Adds miliseconds to current time and returns new value
 *
 * @param ms milliseconds to add
 * @return Time& reference to the current object with added milliseconds
 */
Time& Time::addMsec(int32_t ms)
{
    *this += ms;
    return *this;
}

/**
 * @brief Overloading of assignment operator
 *
 * @param c second Time object
 */
const Time& Time::operator=(const Time& c)
{
    msec_ = c.msec_;
    sec_ = c.sec_;
    hour_ = c.hour_;
    return *this;
}

/**
 * @brief Overloading of addition assignment operator
 *
 * @param c second Time object
 */
Time& Time::operator+=(const Time& c)
{
    normalize(hour_ + c.hour_, sec_ + c.sec_, msec_ + c.msec_);
    return *this;
}

/**
 * @brief Overloading of addition assignment operator with milliseconds
 *
 * @param c milliseconds value
 */
Time& Time::operator+=(const int32_t& c)
{
    normalize(hour_, sec_, msec_ + c);
    return *this;
}

/**
 * @brief Overloading of subtraction assignment operator
 *
 * @param c second Time object
 */
Time& Time::operator-=(const Time& c)
{
    normalize(hour_ - c.hour_, sec_ - c.sec_, msec_ - c.msec_);
    return *this;
}

/**
 * @brief Overloading of subtraction assignment operator with milliseconds
 *
 * @param c milliseconds value
 */
Time& Time::operator-=(const int32_t& c)
{
    normalize(hour_, sec_, msec_ - c);
    return *this;
}

/**
 * @brief Overloading of addition operator
 *
 * @param c second Time object
 * @return result of addition
 */
const Time Time::operator+(const Time& c) const
{
    return Time(*this) += c;
}

/**
 * @brief Overloading of addition operator with milliseconds
 *
 * @param c milliseconds value
 * @return result of addition
 */
const Time Time::operator+(const int32_t& c) const
{
    return Time(*this) += c;
}

/**
 * @brief Overloading of subtraction operator
 *
 * @param c second Time object
 * @return result of subtraction
 */
const Time Time::operator-(const Time& c) const
{
    return Time(*this) -= c;
}

/**
 * @brief Overloading of subtraction operator with milliseconds
 *
 * @param c milliseconds value
 * @return result of subtraction
 */
const Time Time::operator-(const int32_t& c) const
{
    return Time(*this) -= c;
}

/**
 * @brief Overloading of equal operator
 *
 * @param c second Time object
 * @return true if objects is equal otherwise false
 */
bool Time::operator==(const Time& c) const
{
    return (hour_ == c.hour_ && sec_ == c.sec_ && msec_ == c.msec_);
}

/**
 * @brief Overloading of not equal operator
 *
 * @param c second Time object
 * @return true if objects is not equal otherwise false
 */
bool Time::operator!=(const Time& c) const
{
    return !(*this == c);
}

/**
 * @brief Overloading of less than operator
 *
 * @param c second Time object
 * @return true if first object is less than second otherwise false
 */
bool Time::operator<(const Time& c) const
{
    return (hour_ < c.hour_) ||
        (hour_ == c.hour_ && sec_ < c.sec_) ||
        (hour_ == c.hour_ && sec_ == c.sec_ && msec_ < c.msec_);
}

/**
 * @brief Overloading of greater than operator
 *
 * @param c second Time object
 * @return true if first object is greater than second otherwise false
 */
bool Time::operator>(const Time& c) const
{
    return c < *this;
}

/**
 * @brief Overloading of less than or equal operator
 *
 * @param c second Time object
 * @return true if first object is less than or equal second otherwise false
 */
bool Time::operator<=(const Time& c) const
{
    return !(*this > c);
}

/**
 * @brief Overloading of greater than or equal operator
 *
 * @param c second Time object
 * @return true if first object is greater than or equal second otherwise false
 */
bool Time::operator>=(const Time& c) const
{
    return !(*this < c);
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Perform fields normalization and modification to less values
 */
void Time::normalize(int32_t h, int32_t s, int32_t ms)
{
    int32_t tmp = ms / kMilisecondsInSecond;
    s += tmp;
    ms -= tmp * kMilisecondsInSecond;
    if (ms < 0) {
        ms += kMilisecondsInSecond;
        --s;
    }

    tmp = s / kSecondsInHour;
    h += tmp;
    s -= tmp * kSecondsInHour;
    if (s < 0) {
        s += kSecondsInHour;
        --h;
    }

    hour_ = h;
    sec_ = s;
    msec_ = ms;
}

/***************************** END OF FILE ************************************/
