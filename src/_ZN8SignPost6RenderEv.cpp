//cpp
// @symbol _ZN8SignPost6RenderEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SignPost.h"
extern "C" {
}
struct Sub041 {
  virtual void v0(); virtual void v1(); virtual void v2();
  virtual void v3(); virtual void v4(); virtual void v5(int);
};

int SignPost::Render()
{
  if (unk_590 != 0) return 1;
  void* r = *(void**)((char*)&unk_59c);
  if (r != 0) {
    int b = (unk_0b0 & 0x4000) != 0;
    if (b && *(int*)((char*)r+0xc8) != 0) {
      func_ov002_020bb060(((char*)this));
    }
  }
  Sub041* s = (Sub041*)((char*)&mModel);
  s->v5(0);
  return 1;
}
