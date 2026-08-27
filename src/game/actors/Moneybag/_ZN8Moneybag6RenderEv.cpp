//cpp
// @symbol _ZN8Moneybag6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Moneybag.h"
struct Sub { virtual int v0(); virtual int v1(); virtual int v2(); virtual int v3(); virtual int v4(); virtual int m(void*); };

int Moneybag::Render()
{
  bool b = mFlags & 0x40000;
  if(b != 0) return 1;
  if(mState > 1){ ((Sub*)((char*)&mModelAnim))->m(0); }
  if(mState <= 0x1f){ ((Sub*)((char*)&mModel))->m(0); }
  return 1;
}
