#ifndef TIMER_HPP
#define TIMER_HPP

#include "types.h"

struct Timer {
    s64 mTimeBase;
    u8 mIsRunning;
    u8 _padding[3];

    s64 GetTime();
    void StopTimer();
    void StartTimer();
    void ResetTimer();
};

typedef char Timer_SizeMustBe0xC[sizeof(Timer) == 0xc ? 1 : -1];

#endif /* TIMER_HPP */
