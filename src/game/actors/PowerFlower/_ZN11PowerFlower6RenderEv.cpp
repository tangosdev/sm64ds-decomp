//cpp
// @symbol _ZN11PowerFlower6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "PowerFlower.h"
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual void g5(void*); };

int PowerFlower::Render()
{
  int f = (int)((mFlags & 0x40000) != 0);
  if (f != 0) return 1;
  unsigned char st = mLifeTimer;
  if (st < 0x2d && (st & 1)) return 1;
  switch (mState) {
  case 0: ((Sub*)((char*)&mModel1))->g5((char*)&mScaleX); break;
  case 1: ((Sub*)((char*)&mModel2))->g5((char*)&mScaleX); break;
  case 2: ((Sub*)((char*)&mModel2))->g5((char*)&mScaleX); break;
  }
  return 1;
}
