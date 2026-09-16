//cpp
#include "TextureSequence.h"
// @symbol _ZN5Whomp6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Whomp.h"
extern "C" {
}
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(int); };

int Whomp::Render()
{
  if(mShouldRender==0) return 1;
  if(mIsKing!=0)
    mTextureSequence.Update(mModelAnim.data);
  ((Sub*)((char*)&mModelAnim))->g5(0);
  return 1;
}
