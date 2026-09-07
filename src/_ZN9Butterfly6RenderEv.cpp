//cpp
// @symbol _ZN9Butterfly6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Butterfly.h"

int Butterfly::Render()
{
  if(mState == 4) return 1;
  if(mUseAnimModel != 0){
    mModelAnim.Render(0);
  } else {
    mModel.Render((Vector3*)&mScaleX);
  }
  return 1;
}
