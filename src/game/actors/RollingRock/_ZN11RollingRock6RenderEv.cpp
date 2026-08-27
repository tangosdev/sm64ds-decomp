//cpp
// @symbol _ZN11RollingRock6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "RollingRock.h"
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(int); };

int RollingRock::Render()
{
  if(mType >= 2)
    ((Sub*)((char*)&mModel))->g5(0);
  return 1;
}
