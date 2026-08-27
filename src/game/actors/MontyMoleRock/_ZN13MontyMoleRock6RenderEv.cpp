//cpp
// @symbol _ZN13MontyMoleRock6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "MontyMoleRock.h"
extern "C" {
struct Base{ virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(void*); };
}

s32 MontyMoleRock::Render()
{
  Base*b=(Base*)((char*)&(*(Model *)&mModel));
  b->m((char*)&mScaleX);
  return 1;
}
