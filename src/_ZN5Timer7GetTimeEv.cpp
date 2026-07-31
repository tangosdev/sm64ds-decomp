//cpp
#include "Timer.hpp"

extern "C" s64 func_02059650();

s64 Timer::GetTime()
{
    if (!mIsRunning)
        return mTimeBase;
    return func_02059650() - mTimeBase;
}
