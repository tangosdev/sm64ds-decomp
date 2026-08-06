//cpp
// @symbol _ZN5Timer7GetTimeEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Timer.h"
s64 Timer::GetTime()
{
  if (mIsRunning == 0)
    return unk_000;
  return func_02059650() - unk_000;
}
