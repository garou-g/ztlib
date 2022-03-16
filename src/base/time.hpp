/*******************************************************************************
 * @file    time.h
 * @author  garou (xgaroux@gmail.com)
 * @brief   Header file of time measurement.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIME_H
#define __TIME_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

class Time {
public:
    static Time now(void);
    static bool isPast(const Time& start, const Time& delay);
    static bool isPast(const Time& end);
    static uint32_t getSystemTime();
    static bool setSystemTime(uint32_t utc, uint32_t maxDelta);

    static const int32_t kSecondsInHour;
    static const int32_t kMilisecondsInSecond;
    static const int32_t kMilisecondsInHour;

    Time(void);
    Time(int32_t ms);
    Time(int32_t h, int32_t s, int32_t ms);

    bool isZero() const;

    int32_t toSec(void) const;
    int32_t toMsec(void) const;
    int32_t toUTC(void) const;

    int32_t getHour(void) const;
    int32_t getSec(void) const;
    int32_t getMsec(void) const;
    static uint32_t getDeltaUTC(void);
    static void setDeltaUTC(uint32_t delta);

    Time& addSec(int32_t s);
    Time& addMsec(int32_t ms);

    const Time& operator=(const Time& c);
    Time& operator+=(const Time& c);
    Time& operator+=(const int32_t& c);
    Time& operator-=(const Time& c);
    Time& operator-=(const int32_t& c);

    const Time operator+(const Time& c) const;
    const Time operator+(const int32_t& c) const;
    const Time operator-(const Time& c) const;
    const Time operator-(const int32_t& c) const;

    bool operator==(const Time& c) const;
    bool operator!=(const Time& c) const;
    bool operator<(const Time& c) const;
    bool operator>(const Time& c) const;
    bool operator<=(const Time& c) const;
    bool operator>=(const Time& c) const;

private:
    void normalize(void);

    static uint32_t deltaUTC;

    int32_t hour;
    int32_t sec;
    int32_t msec;
};

#endif /* __TIME_H */

/***************************** END OF FILE ************************************/
