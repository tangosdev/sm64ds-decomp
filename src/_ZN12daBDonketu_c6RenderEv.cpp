//cpp
// @symbol _ZN12daBDonketu_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daBDonketu_c.h"
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(int); };

int daBDonketu_c::Render()
{
  if(mNumBulliesKilled >= 3)
    ((Sub*)((char*)&mModelAnim))->g5(0);
  return 1;
}
