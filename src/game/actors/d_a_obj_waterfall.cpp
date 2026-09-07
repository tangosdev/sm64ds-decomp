//cpp
/* Genuine production translation unit for ov002/daObjWaterfall_c
 * (5 function(s)), enrolled as one `complete` delinks span.
 *
 * The file stem follows the snake_case scheme in
 * notes/tu-naming-and-swallowers.md sec 1 (tools/tu_names.py:
 * daObjWaterfall_c -> d_a_obj_waterfall). The class name is unchanged: it is
 * what the ROM's own RTTI record spells.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Consolidates (and replaces) these legacy one-function sources, which this
 * promotion deletes (ROM address order):
 *   [0] 0x020b6e08  src/_ZN16daObjWaterfall_cD1Ev.cpp
 *   [1] 0x020b6e2c  src/_ZN16daObjWaterfall_cD0Ev.cpp
 *   [2] 0x020b6e64  src/_ZN16daObjWaterfall_c8BehaviorEv.cpp
 *   [3] 0x020b6eac  src/_ZN16daObjWaterfall_c13InitResourcesEv.cpp
 *   [4] 0x020b6ee8  src/WaterfallMist_Spawn.cpp
 *
 * daObjWaterfall_c is the ROM-proven identity: the adjacent retail RTTI and
 * type-name records at 0x0210945c/0x02109468 spell it and point at this class's
 * vtable. daObjWaterfall_c_classInit and g_profile_WATERFALL are reconstructed
 * source-style names: the exact original SM64DS spellings do not survive.
 * Historical project aliases were WaterfallMist_Spawn and
 * WaterfallMist_SpawnInfo.
 */

#include "daObjWaterfall_c.h"

extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN8dActor_cC2Ev(void *);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned a, unsigned b, int x, int y, int z, const void *v, void *cb);
extern signed char data_0209f2f8;
}

struct daObjWaterfallSpawnInfo {
    daObjWaterfall_c *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};

typedef char daObjWaterfallSpawnInfo_size_must_be_0x1c[
    sizeof(daObjWaterfallSpawnInfo) == 0x1c ? 1 : -1];

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- class initializer, 0x020b6ee8, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol daObjWaterfall_c_classInit
extern "C" daObjWaterfall_c *daObjWaterfall_c_classInit(void)
{
    daObjWaterfall_c *p =
        (daObjWaterfall_c *)_ZN7fBase_cnwEj(sizeof(daObjWaterfall_c));
    if (p) {
        _ZN8dActor_cC2Ev(p);
        *(int *)p = (int)&_ZTV16daObjWaterfall_c[2];
    }
    return p;
}

/* 0x0210947c..0x02109498, immediately before vtable storage. */
extern "C" daObjWaterfallSpawnInfo g_profile_WATERFALL = {
    daObjWaterfall_c_classInit,
    0x00c5,
    0x0015,
    0x00800003,
    0x00060000,
    0x00300000,
    0x02000000,
    0
};

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- daObjWaterfall_c::InitResources, 0x020b6eac, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjWaterfall_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
int daObjWaterfall_c::InitResources()
{
    signed char v;
    mParticleID = 0x24;
    v = data_0209f2f8;
    if (v != 0x16) {
        if (v == 0x21)
            mParticleID = 0xeb;
    } else {
        mParticleID = 0x71;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- daObjWaterfall_c::Behavior, 0x020b6e64, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjWaterfall_c8BehaviorEv
int daObjWaterfall_c::Behavior()
{
    mParticle = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        (u32)mParticle, mParticleID, mPosX, mPosY, mPosZ, 0, 0);
    return 1;
}

/* ROM ordinals 0/1 -- D1 (0x020b6e08) and D0 (0x020b6e2c) come from the inline
 * destructor in daObjWaterfall_c.h. Defining the destructor out of line here
 * instead makes mwccarm emit D0 before D1, which does not match the ROM span. */
