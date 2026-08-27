//cpp
// @symbol _ZN10ShutterHmc8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "ShutterHmc.h"
extern "C" {
int func_ov002_020bac18(void);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
}

int ShutterHmc::Behavior()
{
  int r = func_ov002_020bac18();
  if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((void*)this), 0, 0))
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((void*)this));
  return r;
}
