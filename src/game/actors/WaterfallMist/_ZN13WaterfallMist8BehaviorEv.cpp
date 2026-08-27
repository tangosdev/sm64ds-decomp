//cpp
#include "WaterfallMist.h"

extern "C" {
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned a, unsigned b, int x, int y, int z, const void* v, void* cb);
}

int WaterfallMist::Behavior()
{
    mParticle = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        (u32)mParticle, mParticleID, mPosX, mPosY, mPosZ, 0, 0);
    return 1;
}
