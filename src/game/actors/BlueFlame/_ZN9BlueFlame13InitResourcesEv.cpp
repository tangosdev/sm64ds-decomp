//cpp
// @symbol _ZN9BlueFlame13InitResourcesEv

#include "BlueFlame.h"

extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *self, dActor_c *actor, Fix12i radius, Fix12i height,
    u32 flags, u32 vulnFlags);

int BlueFlame::InitResources()
{
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mdCcAc_c, this, 0x41000, 0x41000, 0x200002, 0x8000);
    mFlamePos.x = mPosX;
    mFlamePos.y = mPosY;
    mFlamePos.z = mPosZ;
    return 1;
}
