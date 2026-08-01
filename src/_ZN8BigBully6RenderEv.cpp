//cpp
// @symbol _ZN8BigBully6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "BigBully.h"
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(int); };

int BigBully::Render()
{
  if(unk_3fe >= 3)
    ((Sub*)((char*)&mModelAnim))->g5(0);
  return 1;
}
