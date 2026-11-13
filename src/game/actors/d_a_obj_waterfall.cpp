//cpp
/**
 * Waterfall mist.
 *
 * No model. InitResources succeeds with nothing to load; it only
 * picks a particle ID from the current course (gCurrentSublevel, the
 * LEVEL_ID byte): 0x71 on 0x16, 0xeb on 0x21, otherwise 0x24.
 * Behavior (re)issues that particle at this actor's position every
 * frame, feeding last frame's handle back so the emitter is recycled.
 *
 * daObjWaterfall_c_classInit is reconstructed (RTTI daObjWaterfall_c,
 * WATERFALL registry). Retail does not store that spelling.
 *
 * NOTES:
 * - gCurrentSublevel is LEVEL_ID in verified.tsv; a local
 *   `signed char &LEVEL_ID` alias size-DIFFs, so the rename belongs in
 *   symbols.txt.
 * - Behavior passes mPosX/Y/Z as three scalars: z is the first stack
 *   argument, stored on its own rather than through an ldm of mPos.
 */

#include "daObjWaterfall_c.h"
#include "SpawnInfo.h"
#include "Particle__System.h"

extern s8 gCurrentSublevel; /* LEVEL_ID */

enum {
    kCourse16 = 0x16,
    kCourse21 = 0x21,
    kWaterfallMistDefault = 0x24,
    kWaterfallMistCourse16 = 0x71,
    kWaterfallMistCourse21 = 0xeb
};

daObjWaterfall_c *daObjWaterfall_c_classInit() {
    return new daObjWaterfall_c();
}

SpawnInfo<daObjWaterfall_c> g_profile_WATERFALL = {
    daObjWaterfall_c_classInit,
    0x00c5, /* WATERFALL registry id 197 */
    0x0015,
    0x00800003,
    0x00060000,
    0x00300000,
    0x02000000,
    0};

s32 daObjWaterfall_c::InitResources() {
    mParticleID = kWaterfallMistDefault;
    switch (gCurrentSublevel) {
    case kCourse16:
        mParticleID = kWaterfallMistCourse16;
        break;
    case kCourse21:
        mParticleID = kWaterfallMistCourse21;
        break;
    }
    return 1;
}

s32 daObjWaterfall_c::Behavior() {
    mParticleHandle =
        Particle::System::New(mParticleHandle, mParticleID, mPosX, mPosY, mPosZ, 0, 0);
    return 1;
}
