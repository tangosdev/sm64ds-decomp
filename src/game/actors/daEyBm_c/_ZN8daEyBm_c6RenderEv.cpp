//cpp
// @symbol _ZN8daEyBm_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daEyBm_c.h"
extern "C" {
int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    unsigned int a, unsigned int b, int c, int d, int e, const void* f);
}

int daEyBm_c::Render()
{
    mParticle1 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
        mParticle1, 0x46, mPosX, mPosY, mPosZ, 0);
    mParticle2 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
        mParticle2, 0x47, mPosX, mPosY, mPosZ, 0);
    return 1;
}
