//cpp
// @symbol _ZN5Timer9StopTimerEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Timer.h"
void Timer::StopTimer()
{
  if (mIsRunning == 0)
    return;
  mIsRunning = 0;
  mTime = func_02059650() - mTime;
}
