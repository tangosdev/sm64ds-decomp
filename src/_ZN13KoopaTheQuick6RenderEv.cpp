//cpp
// @symbol _ZN13KoopaTheQuick6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "KoopaTheQuick.h"
struct Model {
  virtual void v0();
  virtual void v1();
  virtual void v2();
  virtual void v3();
  virtual void v4();
  virtual void v5(void* p);
  void HideMaterial(int a, int b);
};

int KoopaTheQuick::Render()
{
  Model* m = (Model*)((char*)&mModelAnim);
  m->HideMaterial(0, 1);
  ((Model*)((char*)&mModelAnim))->v5((char*)&mScaleX);
  return 1;
}
