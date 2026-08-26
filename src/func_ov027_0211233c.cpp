//cpp
// @symbol _ZN21SnowmanBreathParticle12CheckSnowmanEv
#include "SnowmanBreath.h"

extern "C" {
s32 Vec3_Dist(const Vector3 *, const Vector3 *);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32, s32, s32, s32);
}

void SnowmanBreathParticle::CheckSnowman()
{
    dActor_c *snowman = dActor_c::FindWithActorID(0x102, 0);
    s32 threshold = mCollider.radius + 0x12b000;
    s32 distance = Vec3_Dist(&mPos, (Vector3 *)&snowman->mPosX);
    if (distance >= threshold)
        return;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
        0x100, mPos.x, mPos.y, mPos.z);
    mTimer = 0;
}
