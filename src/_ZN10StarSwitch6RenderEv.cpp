//cpp
// @symbol _ZN10StarSwitch6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "StarSwitch.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void M(void*); };
struct Sub : Base { };
struct C { char p1[0xd4]; Sub sub; };

int StarSwitch::Render()
{
  ((C*)this)->sub.M((char*)&unk_320);
  return 1;
}
