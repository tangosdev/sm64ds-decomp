//cpp
// @symbol _ZN5Koopa6RenderEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Model.h"
/* recovered: named members + shared header, real C++ method */
#include "Koopa.h"
struct Mdl {
  virtual void v0();
  virtual void v1();
  virtual void v2();
  virtual void v3();
  virtual void v4();
  virtual void slot5(void* p);
};
extern "C" {
struct V3 { int x, y, z; };
}

int Koopa::Render()
{
  volatile struct V3 saved;
  int b = (mFlags & 0x40000) != 0;
  if (b) return 1;
  if (mKoopaVariant == 1) {
    _ZN5Model12ShowMaterialEii(((char*)this) + 0x300, 0, 1);
    _ZN5Model12HideMaterialEii(((char*)this) + 0x300, 0, 2);
  } else {
    _ZN5Model12HideMaterialEii(((char*)this) + 0x300, 0, 1);
    _ZN5Model12ShowMaterialEii(((char*)this) + 0x300, 0, 2);
  }
  saved.x = mScaleX;
  saved.y = mScaleY;
  saved.z = mScaleZ;
  if (mDeathState == 1 && mKoopaVariant == 2) {
    mScaleX = (int)(((long long)*(volatile int*)((char*)&mScaleX) * 0x800 + 0x800) >> 12);
    mScaleY = (int)(((long long)*(volatile int*)((char*)&mScaleY) * 0x800 + 0x800) >> 12);
    mScaleZ = (int)(((long long)*(volatile int*)((char*)&mScaleZ) * 0x800 + 0x800) >> 12);
  }
  ((struct Mdl*)((char*)&mModelAnim))->slot5((char*)&mScaleX);
  mScaleX = saved.x;
  mScaleY = saved.y;
  mScaleZ = saved.z;
  return 1;
}
