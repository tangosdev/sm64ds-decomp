//cpp
// @symbol _ZN5Spiny6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Spiny.h"
struct Obj {
  virtual void m0(); virtual void m1(); virtual void m2();
  virtual void m3(); virtual void m4(); virtual void doit(int);
};

int Spiny::Render()
{
  if((*(unsigned*)((char *)&unk_0b0) & 0x40000) ? 1 : 0) return 1;
  int s=unk_3d8;
  if(s==0 || s==4) ((Obj*)((char *)&mModel))->doit(0);
  else ((Obj*)((char *)&mModelAnim))->doit(0);
  return 1;
}
