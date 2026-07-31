//cpp
#include "Timer.hpp"

extern "C" s64 func_02059650();

void Timer::StopTimer()
{
    if (!mIsRunning)
        return;
    mIsRunning = 0;
    mTimeBase = func_02059650() - mTimeBase;
}
