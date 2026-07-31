//cpp
// @symbol _ZN9PushBlock6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "PushBlock.h"
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual void g5(void*); };

int PushBlock::Render()
{
  int f = (int)((unk_0b0 & 0x40000) != 0);
  if (f != 0) return 1;
  unsigned char st = unk_3ca;
  if (st < 0x2d && (st & 1)) return 1;
  switch (unk_3c0) {
  case 0: ((Sub*)((char*)&mModel1))->g5((char*)&mScaleX); break;
  case 1: ((Sub*)((char*)&mModel2))->g5((char*)&mScaleX); break;
  case 2: ((Sub*)((char*)&mModel2))->g5((char*)&mScaleX); break;
  }
  return 1;
}
