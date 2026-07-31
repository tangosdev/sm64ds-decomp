//cpp
#include "Timer.hpp"

extern "C" s64 func_02059650();

void Timer::StartTimer()
{
    mIsRunning = 1;
    mTimeBase = func_02059650() - mTimeBase;
}
