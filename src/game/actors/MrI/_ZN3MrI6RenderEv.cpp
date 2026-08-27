//cpp
// @symbol _ZN3MrI6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "MrI.h"
extern "C" {
extern int _ZN15TextureSequence6UpdateER15ModelComponents(void*, void*);
}
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(void*); };

int MrI::Render()
{
  _ZN15TextureSequence6UpdateER15ModelComponents(((char*)this)+0x138, ((char*)this)+0xdc);
  ((Sub*)((char*)&mModelAnim))->g5((char*)&mScaleX);
  return 1;
}
