//cpp
// @symbol _ZN5Timer10StartTimerEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Timer.h"
void Timer::StartTimer()
{
  mIsRunning = 1;
  mTime = func_02059650() - mTime;
}
