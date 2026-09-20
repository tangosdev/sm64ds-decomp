//cpp
// @symbol _ZN12daBDonketu_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daBDonketu_c.h"

int daBDonketu_c::Render()
{
  if(mNumBulliesKilled >= 3)
    mModelAnim.Render(0);
  return 1;
}
