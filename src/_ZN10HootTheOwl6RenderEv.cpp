//cpp
// @symbol _ZN10HootTheOwl6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "HootTheOwl.h"
extern "C" {
extern char data_ov094_02136b40[];
}
struct O {
  virtual void m0();
  virtual void m1();
  virtual void m2();
  virtual void m3();
  virtual void m4();
  virtual void m5(int);
};

int HootTheOwl::Render()
{
  if(*(void**)((char *)&mCurrentState) == (void*)data_ov094_02136b40) return 1;
  ((O*)((char *)&mModelAnim))->m5(0);
  return 1;
}
