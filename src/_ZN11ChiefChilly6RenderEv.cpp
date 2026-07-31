//cpp
// @symbol _ZN11ChiefChilly6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "ChiefChilly.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void M(void*); };
struct Sub : Base { };
struct C { char p1[0x30c]; Sub sub; };

int ChiefChilly::Render()
{
  ((C*)this)->sub.M((char*)&mScaleX);
  return 1;
}
