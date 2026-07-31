//cpp
// @symbol _ZN6Cannon6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Cannon.h"
struct Sub {
  virtual void v0();
  virtual void v1();
  virtual void v2();
  virtual void m3();
  virtual void v4();
  virtual void m5(int);
};

int Cannon::Render()
{
  if(unk_180 == 3){
    if(unk_185 >= 3) return 1;
  }
  Sub* o = (Sub*)((char*)&mModel);
  o->m3();
  Sub* o2 = (Sub*)((char*)&mModel);
  o2->m5(0);
  return 1;
}
