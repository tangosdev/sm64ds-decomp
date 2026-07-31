//cpp
// @symbol _ZN14FlameChompFire6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "FlameChompFire.h"
extern "C" {
extern int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(unsigned int a, unsigned int b, int c, int d, int e, void* f);
}

int FlameChompFire::Render()
{
  int b = (unk_0b0 & 0x40000) != 0;
  if (b) return 1;
  unk_324 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
      unk_324, 0x7f, mPosX, mPosY + 0x4b000, mPosZ, 0);
  unk_328 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
      unk_328, 0x80, mPosX, mPosY + 0x4b000, mPosZ, 0);
  return 1;
}
