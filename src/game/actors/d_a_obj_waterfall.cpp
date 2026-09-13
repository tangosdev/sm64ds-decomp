//cpp
/**
 * Waterfall mist.
 *
 * No model. InitResources succeeds with nothing to load; it only
 * picks a particle ID from the current course (data_0209f2f8, the
 * LEVEL_ID byte): 0x71 on 0x16, 0xeb on 0x21, otherwise 0x24.
 * Behavior (re)issues that particle at this actor's position every
 * frame, feeding last frame's handle back so the emitter is recycled.
 *
 * daObjWaterfall_c_classInit is reconstructed (RTTI daObjWaterfall_c,
 * WATERFALL registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: Particle::System::New is a TU-local inline that forwards
 *   to the 5Fix12IiE reconstructed symbol. Declaring the real
 *   Fix12<int> parameters will not convert from s32 mPosX (no ctor)
 *   and the pun/temps DIFF. Declaring int parameters mangle to a
 *   different symbol. Do not put the typed mangled extern on
 *   Particle__System.h.
 * Leftover: data_0209f2f8 is LEVEL_ID in verified.tsv; a local
 *   `signed char &LEVEL_ID` alias size-DIFF 999. Rename is symbols.txt.
 * Leftover: three scalar mPosX/Y/Z into New. A Vector3 overlay of
 *   `&mPosX` size-DIFF 999 (z is the first stack arg, not an ldm).
 */

#include "daObjWaterfall_c.h"

extern "C" s8 data_0209f2f8; /* LEVEL_ID */

extern "C" u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned handle, unsigned effectID, int x, int y, int z,
    const void *dir, void *cb);

namespace Particle {
struct Callback;
/* Forwarder only -- not the layout in Particle__System.h. Inlines to
   the reconstructed 5Fix12IiE symbol so the call spells
   Particle::System::New. See leftover. */
struct System {
    static u32 New(unsigned handle, unsigned effectID,
                   int x, int y, int z, const void *dir, Callback *cb)
    {
        return _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            handle, effectID, x, y, z, dir, cb);
    }
};
}

enum {
    kCourse16 = 0x16,
    kCourse21 = 0x21,
    kWaterfallMistDefault  = 0x24,
    kWaterfallMistCourse16 = 0x71,
    kWaterfallMistCourse21 = 0xeb
};

// @symbol daObjWaterfall_c_classInit
extern "C" daObjWaterfall_c *daObjWaterfall_c_classInit()
{
    return new daObjWaterfall_c();
}

struct WaterfallSpawnInfo {
    daObjWaterfall_c *(*classInit)();
    s16 executePriority; /* +4: also WATERFALL registry id 0x00c5 = 197 */
    s16 renderPriority;  /* +6 */
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};
typedef char WaterfallSpawnInfo_size_must_be_0x1c[
    sizeof(WaterfallSpawnInfo) == 0x1c ? 1 : -1];

// @symbol g_profile_WATERFALL
extern "C" WaterfallSpawnInfo g_profile_WATERFALL = {
    daObjWaterfall_c_classInit,
    0x00c5,
    0x0015,
    0x00800003,
    0x00060000,
    0x00300000,
    0x02000000,
    0
};

// @symbol _ZN16daObjWaterfall_c13InitResourcesEv
s32 daObjWaterfall_c::InitResources()
{
    mParticleID = kWaterfallMistDefault;
    switch (data_0209f2f8) {
    case kCourse16:
        mParticleID = kWaterfallMistCourse16;
        break;
    case kCourse21:
        mParticleID = kWaterfallMistCourse21;
        break;
    }
    return 1;
}

// @symbol _ZN16daObjWaterfall_c8BehaviorEv
s32 daObjWaterfall_c::Behavior()
{
    mParticleHandle = Particle::System::New(
        mParticleHandle, mParticleID, mPosX, mPosY, mPosZ, 0, 0);
    return 1;
}
