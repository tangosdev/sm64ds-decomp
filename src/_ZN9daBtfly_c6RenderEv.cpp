//cpp
// @symbol _ZN9daBtfly_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daBtfly_c.h"

int daBtfly_c::Render()
{
  if(mState == 4) return 1;
  if(mUseAnimModel != 0){
    mModelAnim.Render(0);
  } else {
    mModel.Render((Vector3*)&mScaleX);
  }
  return 1;
}
