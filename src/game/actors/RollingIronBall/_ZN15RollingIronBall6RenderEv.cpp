//cpp
// @symbol _ZN15RollingIronBall6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "RollingIronBall.h"
struct EmbeddedClass {
  virtual void method(void* a);
  virtual void dummy1();
  virtual void dummy2();
  virtual void dummy3();
  virtual void dummy4();
  virtual void virtualMethod(char* a);
};

int RollingIronBall::Render()
{
  unsigned char b = mVariant;
  if(b){
    EmbeddedClass* e = (EmbeddedClass*)((char*)&mModel);
    e->virtualMethod((char*)&mDrawScaleX);
  }
  return 1;
}
