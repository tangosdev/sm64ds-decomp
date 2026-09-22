//cpp
// @symbol _ZN10StarSwitch6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "StarSwitch.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void M(void*); };

int StarSwitch::Render()
{
  ((Base *)&(*(u8 *)&mModel))->M((char *)&mDrawScaleX);
  return 1;
}
