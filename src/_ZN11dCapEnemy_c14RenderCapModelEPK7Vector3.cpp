//cpp
// @symbol _ZN8CapEnemy14RenderCapModelEPK7Vector3
/* recovered: named members + shared header, real C++ method */
#include "CapEnemy.h"
struct Vector3;
struct Vector3;
struct CapModel {
  virtual void f0(); virtual void f1(); virtual void f2();
  virtual void f3(); virtual void f4();
  virtual void render(const Vector3*);
};

void CapEnemy::RenderCapModel(const Vector3 * v)
{
  if((unsigned char)((char*)this)[0x113] >= 6) return;
  CapModel* o=(CapModel*)((char*)&mModel);
  o->render(v);
}
