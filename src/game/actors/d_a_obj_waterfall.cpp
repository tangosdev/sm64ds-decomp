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
 * NOTES:
 * - data_0209f2f8 is LEVEL_ID in verified.tsv; a local
 *   `signed char &LEVEL_ID` alias size-DIFFs, so the rename belongs in
 *   symbols.txt.
 * - Behavior passes mPosX/Y/Z as three scalars: z is the first stack
 *   argument, stored on its own rather than through an ldm of mPos.
 */

#include "daObjWaterfall_c.h"
#include "SpawnInfo.h"
#include "Particle__System.h"

extern s8 data_0209f2f8; /* LEVEL_ID */

enum {
    kCourse16 = 0x16,
    kCourse21 = 0x21,
    kWaterfallMistDefault = 0x24,
    kWaterfallMistCourse16 = 0x71,
    kWaterfallMistCourse21 = 0xeb
};

// @symbol daObjWaterfall_c_classInit
extern "C" daObjWaterfall_c *daObjWaterfall_c_classInit() {
    return new daObjWaterfall_c();
}

// @symbol g_profile_WATERFALL
extern "C" SpawnInfo<daObjWaterfall_c> g_profile_WATERFALL = {
    daObjWaterfall_c_classInit,
    0x00c5, /* WATERFALL registry id 197 */
    0x0015,
    0x00800003,
    0x00060000,
    0x00300000,
    0x02000000,
    0};

// @symbol _ZN16daObjWaterfall_c13InitResourcesEv
s32 daObjWaterfall_c::InitResources() {
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
s32 daObjWaterfall_c::Behavior() {
    mParticleHandle =
        Particle::System::New(mParticleHandle, mParticleID, mPosX, mPosY, mPosZ, 0, 0);
    return 1;
}
