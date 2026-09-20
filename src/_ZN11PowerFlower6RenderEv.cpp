//cpp
// @symbol _ZN11PowerFlower6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "PowerFlower.h"

int PowerFlower::Render()
{
  int f = (int)((mFlags & 0x40000) != 0);
  if (f != 0) return 1;
  unsigned char st = mLifeTimer;
  if (st < 0x2d && (st & 1)) return 1;
  switch (mState) {
  case 0: mModel1.Render((const Vector3 *)&mScaleX); break;
  case 1: mModel2.Render((const Vector3 *)&mScaleX); break;
  case 2: mModel2.Render((const Vector3 *)&mScaleX); break;
  }
  return 1;
}
