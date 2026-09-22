//cpp
// @symbol _ZN5Stage7LoadFogEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Stage.h"
/* Stage::LoadFog. Lays down two default ramps -- a shallow one in mFog[0] and
   a steeper one in mFog[1] -- then lets the level's own fog records, if it has
   any, overwrite them through Fog::Init. */
extern "C" {
extern void _ZN3Fog4InitEt5Fix12IiES1_(Fog *self, unsigned short color, int nearv, int farv);
}

void Stage::LoadFog()
{
  unsigned char a = 0;
  unsigned char b = 0x28;
  int i;
  for (i = 0; i < 0x20; i++)
  {
    mFog[0].mDensity[i] = a;
    mFog[1].mDensity[i] = b;
    a += 4;
    b += 2;
  }
  mFog[0].mEnabled = 0;
  mFog[1].mEnabled = 1;
  mFog[1].mShift = 6;
  mFog[1].mOffset = 0;
  mFog[1].mColor = 0x6000;
  if (data_0209f258 == 0)
  {
    return;
  }
  {
    unsigned char *src = (unsigned char *) data_0209f31c;
    Fog *dst = mFog;
    int j;
    for (j = 0; j < data_0209f258; j++)
    {
      if (src[0] != 1)
      {
        dst->mEnabled = 0;
      }
      else
      {
        unsigned short color = ((src[1] >> 3) | ((src[2] >> 3) << 5)) | ((src[3] >> 3) << 10);
        _ZN3Fog4InitEt5Fix12IiES1_(dst, color, *((unsigned short *) (src + 4)), *((unsigned short *) (src + 6)));
      }
      src += 8;
      dst++;
    }
  }
}
