//cpp
// @symbol _ZN5Spiny6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Spiny.h"
int Spiny::Render()
{
  if((mFlags & 0x40000) ? 1 : 0) return 1;
  int s=mState;
  if(s==0 || s==4) mModel.Render(0);
  else mModelAnim.Render(0);
  return 1;
}
