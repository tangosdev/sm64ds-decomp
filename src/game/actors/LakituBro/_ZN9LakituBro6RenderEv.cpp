//cpp
// @symbol _ZN9LakituBro6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "LakituBro.h"
extern "C" {
extern int _ZN15TextureSequence6UpdateER15ModelComponents(void*, void*);
}
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(void*); };

int LakituBro::Render()
{
  if (unk_2dc == 1) return 1;
  _ZN15TextureSequence6UpdateER15ModelComponents(((char*)this)+0x1d8, ((char*)this)+0x118);
  ((Sub*)((char*)&mModelAnim1))->g5(0);
  return 1;
}
