//cpp
// @symbol _ZN16daKing_Donketu_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daKing_Donketu_c.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void M(void*); };

int daKing_Donketu_c::Render()
{
  ((Base *)&mBlendModelAnim)->M((char*)&mScaleX);
  return 1;
}
