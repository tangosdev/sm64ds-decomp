//cpp
// @symbol _ZN8daKpFr_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daKpFr_c.h"
extern "C" {
extern int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(unsigned int a, unsigned int b, int c, int d, int e, void* f);
}

int daKpFr_c::Render()
{
  int b = (mFlags & 0x40000) != 0;
  if (b) return 1;
  mParticle1 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
      mParticle1, 0x7f, mPosX, mPosY + 0x4b000, mPosZ, 0);
  mParticle2 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
      mParticle2, 0x80, mPosX, mPosY + 0x4b000, mPosZ, 0);
  return 1;
}
