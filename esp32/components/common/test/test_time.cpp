#include <sys/time.h>

#include "unity.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "time.hpp"

struct TestTimeData {
    Time t;
    int32_t i;
};

TEST_CASE("constructor", "[time]")
{
    Time t(0);
    TEST_ASSERT(t == 0);

    t = 0;
    TEST_ASSERT(t == 0);

    t = { 1, 2, 3 };
    TEST_ASSERT(t == Time(1, 2, 3));

    t = Time(3, 2, 1);
    TEST_ASSERT(t == Time(3, 2, 1));
}

TEST_CASE("getDeltaUTC", "[time]")
{
    const int utc = 1234;
    Time t1 = 0, t2 = 0;

    t1.setDeltaUTC(0);
    TEST_ASSERT(t1.getDeltaUTC() == 0);
    TEST_ASSERT(t2.getDeltaUTC() == 0);

    t1.setDeltaUTC(utc);
    TEST_ASSERT(t1.getDeltaUTC() == utc);
    TEST_ASSERT(t2.getDeltaUTC() == utc);
}

TEST_CASE("isZero", "[time]")
{
    Time t = 0;
    TEST_ASSERT(t.isZero());

    t = { 1, 2, 3 };
    TEST_ASSERT(!t.isZero());
}

TEST_CASE("toSec", "[time]")
{
    TestTimeData testTime[] = {
        { {  0,  2,  3 },   2 },
        { {  0, -2,  3 },  -2 },
        { {  1,  0,  3 },  3600 },
        { { -1,  0,  3 }, -3600 },
        { {  1,  2,  3 },  3602 },
        { {  1, -2,  3 },  3598 },
        { { -1,  2,  3 }, -3598 },
        { { -1, -2,  3 }, -3602 },
        { {  1, 3602,  3 }, 7202 },
        { {  1, -3602,  3 }, -2 },
        { { -1, 3602,  3 }, 2 },
        { { -1, -3602,  3 }, -7202 },
    };

    const int size = sizeof(testTime) / sizeof(TestTimeData);
    int32_t resT[size], resI[size];
    for (int i = 0; i < size; ++i) {
        resT[i] = testTime[i].t.toSec();
        resI[i] = testTime[i].i;
    }
    TEST_ASSERT_EQUAL_INT_ARRAY(resI, resT, size);
}

TEST_CASE("toMsec", "[time]")
{
    TestTimeData testTime[] = {
        { {  0,  2,  3 },        2003 },
        { {  0, -2,  3 },       -1997 },
        { {  1,  0,  3 },     3600003 },
        { { -1,  0,  3 },    -3599997 },
        { {  1,  2,  3 },     3602003 },
        { {  1, -2,  3 },     3598003 },
        { { -1,  2,  3 },    -3597997 },
        { { -1, -2,  3 },    -3601997 },
        { {  1, 3602,  3 },   7202003 },
        { {  1, -3602,  3 },    -1997 },
        { { -1, 3602,  3 },      2003 },
        { { -1, -3602,  3 }, -7201997 },
        { {  1,  2,  1003 },  3603003 },
        { {  1,  2, -1003 },  3600997 },
    };

    const int size = sizeof(testTime) / sizeof(TestTimeData);
    int32_t resT[size], resI[size];
    for (int i = 0; i < size; ++i) {
        resT[i] = testTime[i].t.toMsec();
        resI[i] = testTime[i].i;
    }
    TEST_ASSERT_EQUAL_INT_ARRAY(resI, resT, size);
}

TEST_CASE("toUTC", "[time]")
{
    TestTimeData testTime[] = {
        { {  0,  2,  3 },           2 },
        { {  0, -2,  3 },          -2 },
        { {  1,  0,  3 },        3600 },
        { { -1,  0,  3 },       -3600 },
        { {  1,  2,  3 },        3602 },
        { {  1, -2,  3 },        3598 },
        { { -1,  2,  3 },       -3598 },
        { { -1, -2,  3 },       -3602 },
        { {  1, 3602,  3 },      7202 },
        { {  1, -3602,  3 },       -2 },
        { { -1, 3602,  3 },         2 },
        { { -1, -3602,  3 },    -7202 },
    };

    Time::setDeltaUTC(5);

    const int size = sizeof(testTime) / sizeof(TestTimeData);
    int32_t resT[size], resI[size];
    for (int i = 0; i < size; ++i) {
        resT[i] = testTime[i].t.toUTC();
        resI[i] = testTime[i].i + 5;
    }
    TEST_ASSERT_EQUAL_INT_ARRAY(resI, resT, size);
}

TEST_CASE("getHour", "[time]")
{
    TestTimeData testTime[] = {
        { {  1,  0,  0 },           1 },
        { { -1,  0,  0 },          -1 },
        { {  0,  3601,  0 },        1 },
        { {  0, -3601,  0 },       -2 },
        { {  1,  10803,  0 },       4 },
        { {  1, -10803,  0 },      -3 },
        { {  0,  0,  10803100 },    3 },
        { {  0,  0, -10803100 },   -4 },
    };

    const int size = sizeof(testTime) / sizeof(TestTimeData);
    int32_t resT[size], resI[size];
    for (int i = 0; i < size; ++i) {
        resT[i] = testTime[i].t.getHour();
        resI[i] = testTime[i].i;
    }
    TEST_ASSERT_EQUAL_INT_ARRAY(resI, resT, size);
}

TEST_CASE("getSec", "[time]")
{
    TestTimeData testTime[] = {
        { {  1,  0,  0 },           0 },
        { { -1,  0,  0 },           0 },
        { {  1,  3601,  0 },        1 },
        { {  0, -3601,  0 },     3599 },
        { {  1,  10803,  0 },       3 },
        { {  1, -10803,  0 },    3597 },
        { {  0,  0,  10803100 },    3 },
        { {  0,  0, -10803100 }, 3596 },
    };

    const int size = sizeof(testTime) / sizeof(TestTimeData);
    int32_t resT[size], resI[size];
    for (int i = 0; i < size; ++i) {
        resT[i] = testTime[i].t.getSec();
        resI[i] = testTime[i].i;
    }
    TEST_ASSERT_EQUAL_INT_ARRAY(resI, resT, size);
}

TEST_CASE("getMsec", "[time]")
{
    TestTimeData testTime[] = {
        { {  1,  0,  1 },           1 },
        { { -1,  0,  2 },           2 },
        { {  1,  3601,  3 },        3 },
        { {  0, -3601,  4 },        4 },
        { {  1,  10803,  5 },       5 },
        { {  1, -10803,  6 },       6 },
        { {  0,  0,  10803100 },  100 },
        { {  0,  0, -10803100 },  900 },
    };

    const int size = sizeof(testTime) / sizeof(TestTimeData);
    int32_t resT[size], resI[size];
    for (int i = 0; i < size; ++i) {
        resT[i] = testTime[i].t.getMsec();
        resI[i] = testTime[i].i;
    }
    TEST_ASSERT_EQUAL_INT_ARRAY(resI, resT, size);
}

TEST_CASE("addSec", "[time]")
{
    Time t1 = 0;

    t1.addSec(60);
    TEST_ASSERT(t1.getSec() == 60);
    t1.addSec(-30);
    TEST_ASSERT(t1.getSec() == 30);

    t1 = 0;
    t1.addSec(3700);
    TEST_ASSERT(t1.getSec() == 100);
    TEST_ASSERT(t1.getHour() == 1);
}

TEST_CASE("addMsec", "[time]")
{
    Time t1 = 0;

    t1.addMsec(60);
    TEST_ASSERT(t1.getMsec() == 60);
    t1.addMsec(-30);
    TEST_ASSERT(t1.getMsec() == 30);

    t1 = 0;
    t1.addMsec(3750);
    TEST_ASSERT(t1.getMsec() == 750);
    TEST_ASSERT(t1.getSec() == 3);
}

TEST_CASE("operator=", "[time]")
{
    Time t1 = 0, t2(1, 2, 3);
    t1 = t2;
    TEST_ASSERT(t1 == t2);
}

TEST_CASE("operator+=", "[time]")
{
    Time t1 = 0;
    t1 += 100;
    TEST_ASSERT(t1.getMsec() == 100);
    t1 += Time(1, 0, 0);
    TEST_ASSERT(t1 == Time(1, 0, 100));
    t1 += 1000;
    TEST_ASSERT(t1 == Time(1, 1, 100));
}

TEST_CASE("operator-=", "[time]")
{
    Time t1(1, 1, 100);
    t1 -= 100;
    TEST_ASSERT(t1.getMsec() == 0);
    t1 -= Time(1, 0, 0);
    TEST_ASSERT(t1 == Time(0, 1, 0));
    t1 -= 1000;
    TEST_ASSERT(t1 == Time(0, 0, 0));
}

TEST_CASE("operator+", "[time]")
{
    Time t1(1, 2, 3), t2(3, 2, 1);
    TEST_ASSERT(t1 + t2 == Time(4, 4, 4));
}

TEST_CASE("operator-", "[time]")
{
    Time t1(1, 2, 3), t2(3, 2, 1);
    TEST_ASSERT(t1 - t2 == Time(-2, 0, 2));

    Time t3(10, 60, 200), t4(5, 5, 400);
    TEST_ASSERT(t3 - t4 == Time(5, 54, 800));
}

TEST_CASE("operator==", "[time]")
{
    Time t1(1, 2, 3), t2(1, 2, 3);
    TEST_ASSERT(t1 == t2);
}

TEST_CASE("operator!=", "[time]")
{
    Time t1(1, 2, 3), t2(3, 2, 1);
    TEST_ASSERT(t1 != t2);
}

TEST_CASE("operator<", "[time]")
{
    Time t1(1, 2, 3), t2(3, 2, 1);
    TEST_ASSERT(t1 < t2);
}

TEST_CASE("operator>", "[time]")
{
    Time t1(1, 2, 3), t2(3, 2, 1);
    TEST_ASSERT(t2 > t1);
}

TEST_CASE("operator<=", "[time]")
{
    Time t1(1, 2, 3), t2(3, 2, 1), t3(1, 2, 3);
    TEST_ASSERT(t1 <= t2);
    TEST_ASSERT(t1 <= t3);
}

TEST_CASE("operator>=", "[time]")
{
    Time t1(1, 2, 3), t2(3, 2, 1), t3(3, 2, 1);
    TEST_ASSERT(t2 >= t2);
    TEST_ASSERT(t2 >= t3);
}

TEST_CASE("now", "[time]")
{
    struct timeval tvNow;

    gettimeofday(&tvNow, NULL);
    Time t = Time::now();
    TEST_ASSERT(t.toSec() == tvNow.tv_sec);
    TEST_ASSERT(t.toMsec() == (tvNow.tv_sec * 1000 + tvNow.tv_usec / 1000));

    vTaskDelay(pdMS_TO_TICKS(100));

    gettimeofday(&tvNow, NULL);
    t = Time::now();
    TEST_ASSERT(t.toSec() == tvNow.tv_sec);
    TEST_ASSERT(t.toMsec() == (tvNow.tv_sec * 1000 + tvNow.tv_usec / 1000));
}

TEST_CASE("isPast", "[time]")
{
    Time t1, t2;
    t1 = t2 = Time::now();
    t2 += 150;

    vTaskDelay(pdMS_TO_TICKS(100));
    TEST_ASSERT(Time::isPast(t1, 50));
    TEST_ASSERT(!Time::isPast(t1, 150));
    TEST_ASSERT(Time::isPast(t1));
    TEST_ASSERT(!Time::isPast(t2));

    vTaskDelay(pdMS_TO_TICKS(100));
    TEST_ASSERT(Time::isPast(t1, 50));
    TEST_ASSERT(Time::isPast(t1, 150));
    TEST_ASSERT(Time::isPast(t1));
    TEST_ASSERT(Time::isPast(t2));
}

TEST_CASE("getSystemTime", "[time]")
{
    Time::setSystemTime(1200, 0);
    Time t = Time::now();
    TEST_ASSERT(Time::getSystemTime() == t.toUTC());
}
