//cpp
// @symbol _ZN8YoshiEgg6RenderEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Player.h"
/* recovered: named members + shared header, real C++ method */
#include "YoshiEgg.h"
struct Obj {
  virtual void v0();
  virtual void v1();
  virtual void v2();
  virtual void v3();
  virtual void v4();
  virtual void m(int a);
};
extern "C" {
}

int YoshiEgg::Render()
{
  int b = (int)((mFlags & 0x40000) != 0);
  if(b) return 1;
  if(_ZN6Player16IsInsideOfCannonEv(*(void**)((char*)&mPlayer))) return 1;
  if(*(unsigned char*)(*(char**)((char*)&mPlayer)+0x6f5) < 1) return 1;
  ((Obj*)((char*)&mModelAnim))->m(0);
  return 1;
}
