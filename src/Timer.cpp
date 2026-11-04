//cpp
/* leftovers:
 * #pragma defer_codegen off stays. It emits .text in source order, which is
 * the ROM order 0x02019584..0x02019664. func_0201964c stays a free function;
 * Timer::Timer() would rename the symbol. OS_GetTick is the SDK tick read
 * (timer 0 at 0x4000100 plus the counter at data_020a6438).
 * local extern: decl_common.h declares it, and also `extern int data_0209d4b8`.
 * This TU defines that object, so the header cannot be included here.
 */
/* Timer: arm9 stopwatch. mTime is the start tick while running and the
 * frozen elapsed count while stopped.
 */
#include "Timer.h"

extern "C" s64 OS_GetTick(void);

#pragma defer_codegen off

/* The shared random seed RandomIntInternal draws from. One s32, seeded to 1.
   Its empty destructor is the vague _ZN7RngSeedD1Ev this file emits;
   canonical-duplicate maps that onto func_0201973c, which stays in
   func_020196cc.c. */
extern "C" void func_0203b9b4(int *p, int v);
extern "C" Timer *func_0201964c(Timer *timer);

struct RngSeed {
    s32 value;
    RngSeed() {
        func_0203b9b4(&value, 1);
    }
    ~RngSeed() {}
};

RngSeed data_0209d4b8;
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

// @symbol _ZN5Timer7GetTimeEv
s64 Timer::GetTime()
{
    if (!mIsRunning)
        return mTime;
    return OS_GetTick() - mTime;
}

// @symbol _ZN5Timer9StopTimerEv
void Timer::StopTimer()
{
    if (!mIsRunning)
        return;
    mIsRunning = 0;
    mTime = OS_GetTick() - mTime;
}

// @symbol _ZN5Timer10StartTimerEv
void Timer::StartTimer()
{
    mIsRunning = 1;
    mTime = OS_GetTick() - mTime;
}

// @symbol _ZN5Timer10ResetTimerEv
void Timer::ResetTimer()
{
    mIsRunning = 0;
    mTime = 0;
}

/* The C1 constructor (ResetTimer, then return this) stays an extern "C"
   helper under its address name; the Timer::Timer() spelling would rename
   the symbol. */
// @symbol func_0201964c
extern "C" Timer *func_0201964c(Timer *__this)
{
    __this->ResetTimer();
    return __this;
}
