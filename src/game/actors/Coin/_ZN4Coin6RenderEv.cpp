//cpp
// @symbol _ZN4Coin6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Coin.h"
extern "C" int _ZN11CommonModel6RenderEPK7Vector3(void*, void*);
struct Flags { unsigned char b0:1; };

int Coin::Render()
{
  int f;
  int b;
  if (!((struct Flags*)((char*)&mCoinFlags))->b0) return 1;
  f = mFlags;
  b = (f & 0x40000) != 0;
  if (b) return 1;
  {
    unsigned short x = mDisappearTimer;
    if (x < 0x2d && (x & 1)) return 1;
  }
  if (!(f & 0x10))
    _ZN11CommonModel6RenderEPK7Vector3(((char*)this)+0xd8, 0);
  else
    _ZN11CommonModel6RenderEPK7Vector3(((char*)this)+0x114, 0);
  return 1;
}
