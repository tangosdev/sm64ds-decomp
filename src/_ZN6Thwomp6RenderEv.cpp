//cpp
// @symbol _ZN11daDsnBase_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daDsnBase_c.h"
extern "C" {
extern int _ZN15TextureSequence6UpdateER15ModelComponents(void*, void*);
}
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(int); };

int daDsnBase_c::Render()
{
  int check = *(int*)(mFileTable+0xc);
  if (check != 0)
    _ZN15TextureSequence6UpdateER15ModelComponents(((char*)this)+0x324, ((char*)this)+0xdc);
  ((Sub*)((char*)&mModel))->g5(0);
  return 1;
}
