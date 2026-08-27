//cpp
// @symbol _ZN6Eyerok6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Eyerok.h"
extern "C" {
extern void _ZN15TextureSequence6UpdateER15ModelComponents(void* thiz, void* mc);
extern unsigned char data_ov066_0211ae04;
}
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(void*); };

int Eyerok::Render()
{
  if (mPartIdx == 0) {
    if (data_ov066_0211ae04 == 1) {
      ((Sub*)&mModel2)->g5(0);
    }
    return 1;
  }
  if (data_ov066_0211ae04 == 1) return 1;
  _ZN15TextureSequence6UpdateER15ModelComponents(&mTextureSequence, &mBlendModelAnim.data);
  ((Sub*)&mBlendModelAnim)->g5(0);
  return 1;
}
