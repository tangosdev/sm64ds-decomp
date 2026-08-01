//cpp
// @symbol _ZN5Timer7GetTimeEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Timer.h"
typedef long long s64;

s64 Timer::GetTime()
{
  if (mIsRunning == 0)
    return *(s64*)((char *)this);
  return func_02059650() - *(s64*)((char *)this);
}
