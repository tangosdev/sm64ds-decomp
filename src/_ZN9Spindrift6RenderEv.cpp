//cpp
// @symbol _ZN9Spindrift6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Spindrift.h"
struct Obj { virtual void m0(); virtual void m1(); virtual void m2(); virtual void m3(); virtual void m4(); virtual void m5(int); };

int Spindrift::Render()
{
  int b = (unk_0b0 & 0x40000) ? 1 : 0; if (b) return 1;
  Obj* o = (Obj*)((char*)&mModelAnim);
  o->m5(0);
  return 1;
}
