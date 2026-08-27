//cpp
// @symbol _ZN5Crate6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Crate.h"
struct Sub {
  virtual void v0();
  virtual void v1();
  virtual void v2();
  virtual void v3();
  virtual void v4();
  virtual void m5(int);
};

int Crate::Render()
{
  if(mState == 6) return 1;
  {
    int b = (int)((mFlags & 0x40000) != 0);
    if(b) return 1;
  }
  Sub* o = (Sub*)&mModel;
  o->m5(0);
  return 1;
}
