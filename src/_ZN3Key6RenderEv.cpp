//cpp
// @symbol _ZN3Key6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Key.h"
extern "C" {
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(void*); };
extern char data_ov089_021328b4[];
}

int Key::Render()
{
  int b = (int)((unk_0b0 & 0x40000) != 0);
  if (b) return 1;
  if (unk_448 != 0) {
    ((Sub*)((char*)&mModelAnim))->m(0);
  } else {
    ((Sub*)((char*)&mModelAnim))->m((char*)&mScaleX);
    if (*(int*)((char*)data_ov089_021328b4 + (mState << 2)) != 0 && unk_448 == 0) {
      ((Sub*)((char*)&mModel))->m(0);
    }
  }
  return 1;
}
