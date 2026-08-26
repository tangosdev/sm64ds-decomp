//cpp
// @symbol _ZN21SnowmanBreathParticle6RenderEv
#include "SnowmanBreath.h"

extern "C" u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32, u32, s32, s32, s32, const void *, void *);

void SnowmanBreathParticle::Render()
{
    if (mTimer == 0)
        return;
    mParticleID =
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            mParticleID, 0x111, mPos.x, mPos.y, mPos.z, 0, 0);
}
