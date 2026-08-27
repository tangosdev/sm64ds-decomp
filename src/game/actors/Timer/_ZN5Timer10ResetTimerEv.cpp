//cpp
// @symbol _ZN5Timer10ResetTimerEv
/* recovered: named members + shared header, real C++ method */
#include "Timer.h"


void Timer::ResetTimer()
{
    mIsRunning = 0;
    mTime = 0;
}
