//cpp
/**
 * Invisible climbable pole.
 *
 * No model. Mario grabs a cylinder. Height is the low byte of param1
 * (tenths of a unit, minus 10) in Fix12. Bit 8 of param1 makes the
 * cylinder hurt on contact.
 *
 * deslop
 */

#include "daBar_c.h"

enum {
    kHeightParamBias = 0xa,       /* subtracted from the param byte first */
    kHeightTenth = 0xa,           /* then ×10, like N64 poles */
    kMinHeightFix12 = 0x1000,     /* 1.0 if that underflowed */
    kClipPadFix12 = 0x640000,     /* extra clip past half-height */
    kCylinderRadiusFix12 = 0x35555,
    kParamHurtBit = 0x100,        /* param1 bit 8: cylinder hurts */
    kClsnFlags = 0x0080000c,
    kClsnFlagsHurt = 0x0280000c
};

extern "C" daBar_c *daBar_c_classInit();

// @symbol daBar_c_classInit
extern "C" daBar_c *daBar_c_classInit()
{
    return new daBar_c;
}

extern "C" DaBarSpawnInfo g_profile_BAR = {
    daBar_c_classInit,
    0x011f,       /* profile id packed with execute order */
    0x0099,       /* draw order */
    0x00000003,   /* actorFlags */
    0,
    0,
    0,
    0
};

// @symbol _ZN7daBar_c13InitResourcesEv
s32 daBar_c::InitResources()
{
    s32 height = (((param1 & 0xff) - kHeightParamBias) * kHeightTenth) << 12;
    if (height <= 0)
        height = kMinHeightFix12;
    s32 halfHeight = height >> 1;

    SetRanges(halfHeight, halfHeight, halfHeight + kClipPadFix12, 0);
    mClsn.Init(this, kCylinderRadiusFix12, height,
        (param1 & kParamHurtBit) ? kClsnFlagsHurt : kClsnFlags, 0);
    return 1;
}

// @symbol _ZN7daBar_c8BehaviorEv
s32 daBar_c::Behavior()
{
    mClsn.Clear();
    mClsn.Update();
    return 1;
}

// @symbol _ZN7daBar_c6RenderEv
s32 daBar_c::Render()
{
    return 1;
}

// @symbol _ZN7daBar_c16OnPendingDestroyEv
void daBar_c::OnPendingDestroy()
{
}

// @symbol _ZN7daBar_c16CleanupResourcesEv
s32 daBar_c::CleanupResources()
{
    return 1;
}
