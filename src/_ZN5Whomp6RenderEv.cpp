//cpp
#include "TextureSequence.h"
// @symbol _ZN5Whomp6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Whomp.h"

int Whomp::Render()
{
  if(mShouldRender==0) return 1;
  if(mIsKing!=0)
    mTextureSequence.Update(mModelAnim.data);
  mModelAnim.Render(0);
  return 1;
}
