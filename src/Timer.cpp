//cpp
/* leftovers:
 * #pragma defer_codegen off stays. It emits .text in source order, which is
 * the ROM order 0x02019584..0x02019664. func_0201964c stays a free function;
 * Timer::Timer() would rename the symbol. OS_GetTick is the SDK tick read
 * (timer 0 at 0x4000100 plus the counter at data_020a6438).
 */
/* Timer: arm9 stopwatch. mTime is the start tick while running and the
 * frozen elapsed count while stopped.
 */
#include "Timer.h"
#include "nitro/os/tick.h"

#pragma defer_codegen off

/* The shared random state the minigames draw from: a second LevelRng,
   seeded to 1 like data_0209e650. The constructor is defined inline here
   the way func_0201fec8.cpp defines it. The destructor is only declared,
   so this file does not emit it: retail registers func_0201973c, the empty
   destructor defined in func_020196cc.c. */
extern "C" void func_0203b9b4(int *p, int v);
extern "C" Timer *func_0201964c(Timer *timer);
#include "LevelRng.h"

inline LevelRng::LevelRng() {
    func_0203b9b4(&value, 1);
}

LevelRng data_0209d4b8;
Timer gRaceTimer;

/* Sequences the Timer construction after the seed's destructor registration.
   Timer has no constructor: Timer::Timer() would rename func_0201964c.
   The call sits above func_0201964c's body so it is not inlined. The object
   itself is unused storage (the call does not touch this) and is deadstripped. */
struct TimerInit {
    TimerInit() {
        func_0201964c(&gRaceTimer);
    }
};
static TimerInit sTimerInit;

s64 Timer::GetTime()
{
    if (!mIsRunning)
        return mTime;
    return OS_GetTick() - mTime;
}

void Timer::StopTimer()
{
    if (!mIsRunning)
        return;
    mIsRunning = 0;
    mTime = OS_GetTick() - mTime;
}

void Timer::StartTimer()
{
    mIsRunning = 1;
    mTime = OS_GetTick() - mTime;
}

void Timer::ResetTimer()
{
    mIsRunning = 0;
    mTime = 0;
}

/* The C1 constructor (ResetTimer, then return this) stays an extern "C"
   helper under its address name; the Timer::Timer() spelling would rename
   the symbol. */
extern "C" Timer *func_0201964c(Timer *__this)
{
    __this->ResetTimer();
    return __this;
}
