//cpp
// @symbol _ZN11RollingRock6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "RollingRock.h"

int RollingRock::Render()
{
  if(mType >= 2)
    mModel.Render(0);
  return 1;
}
