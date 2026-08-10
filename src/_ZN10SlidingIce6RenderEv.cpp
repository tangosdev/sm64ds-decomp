//cpp
// @symbol _ZN10SlidingIce6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "SlidingIce.h"
struct VObj {
  virtual void f0();
  virtual void f1();
  virtual void f2();
  virtual void f3();
  virtual void f4();
  virtual void m5(int);
};
extern "C" {
void _ZN8Platform21UpdateModelPosAndRotYEv(void*);
void _ZN8Platform19UpdateClsnPosAndRotEv(void*);
}

int SlidingIce::Render()
{
  int x = actorID==0x5d;
  if(x){
    _ZN8Platform21UpdateModelPosAndRotYEv(((char*)this));
    _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
    ((VObj*)((char*)&mModel))->m5(0);
  }
  return 1;
}
