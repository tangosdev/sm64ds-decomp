//cpp
/* Production translation unit, manually curated.
 * ov002/WaterfallMist  (5 function(s))
 *
 * ENROLLED AND CANONICAL. The module's delinks.txt licenses the .text range
 * below to this path, so the ROM build compiles this file in place of the
 * legacy one-function sources listed at the foot of this comment.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x020b6e08  src/_ZN13WaterfallMistD1Ev.cpp
 *   [1] 0x020b6e2c  src/_ZN13WaterfallMistD0Ev.cpp
 *   [2] 0x020b6e64  src/_ZN13WaterfallMist8BehaviorEv.cpp
 *   [3] 0x020b6eac  src/_ZN13WaterfallMist13InitResourcesEv.cpp
 *   [4] 0x020b6ee8  src/WaterfallMist_Spawn.c
 *
 * Naming boundary: these configured text symbols use WaterfallMist, while the
 * adjacent retail RTTI spells daObjWaterfall_c. Until that symbol migration is
 * resolved, this TU claims only the five verified text contributions; its
 * generated RTTI/vtable is evidence, not licensed retail data.
 */

#include "WaterfallMist.h"

extern "C" {
extern int _ZTV13WaterfallMist[];
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN8dActor_cC2Ev(void *);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned a, unsigned b, int x, int y, int z, const void *v, void *cb);
extern signed char data_0209f2f8;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- WaterfallMist_Spawn, 0x020b6ee8, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol WaterfallMist_Spawn
extern "C" WaterfallMist *WaterfallMist_Spawn(void)
{
    WaterfallMist *p = (WaterfallMist *)_ZN7fBase_cnwEj(sizeof(WaterfallMist));
    if (p) {
        _ZN8dActor_cC2Ev(p);
        *(int *)p = (int)&_ZTV13WaterfallMist[2];
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN13WaterfallMist13InitResourcesEv, 0x020b6eac, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13WaterfallMist13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
int WaterfallMist::InitResources()
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
/* ROM ordinal 2 -- _ZN13WaterfallMist8BehaviorEv, 0x020b6e64, size 0x48 */
/* -------------------------------------------------------------------------- */
int WaterfallMist::Behavior()
{
    mParticle = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        (u32)mParticle, mParticleID, mPosX, mPosY, mPosZ, 0, 0);
    return 1;
}

/* ROM ordinals 0/1 -- the compiler emits D1 and D0 from this definition. */
