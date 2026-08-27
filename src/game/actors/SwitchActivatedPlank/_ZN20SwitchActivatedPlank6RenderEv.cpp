//cpp
// @symbol _ZN20SwitchActivatedPlank6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "SwitchActivatedPlank.h"
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void v5(int); };

int SwitchActivatedPlank::Render()
{
  if (mVisible != 0) {
    Sub *s = (Sub*)((char *)&mModel2);
    s->v5(0);
  }
  return 1;
}
