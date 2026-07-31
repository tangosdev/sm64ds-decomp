//cpp
// @symbol _ZN5Whomp6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Whomp.h"
extern "C" {
extern int _ZN15TextureSequence6UpdateER15ModelComponents(void*, void*);
}
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(int); };

int Whomp::Render()
{
  if(unk_404==0) return 1;
  if(mIsKing!=0)
    _ZN15TextureSequence6UpdateER15ModelComponents(((char*)this)+0x330, ((char*)this)+0x2d4);
  ((Sub*)((char*)&mModelAnim))->g5(0);
  return 1;
}
