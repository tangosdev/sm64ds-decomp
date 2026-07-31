//cpp
// @symbol _ZN15FaderBrightness20IsBetweenStartAndEndEv
/* recovered: named members + shared header, real C++ method */
#include "FaderBrightness.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual int a(); virtual int b(); };

int FaderBrightness::IsBetweenStartAndEnd()
{
  if(((Base *)this)->a()==0 && ((Base *)this)->b()==0) return 1;
  return 0;
}
