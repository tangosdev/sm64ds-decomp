//cpp
/* Genuine production translation unit for ov002/daBar_c.
 *
 * The eight functions are defined in reverse ROM order because mwccarm emits
 * ordinary function sections in reverse source order. InitResources is the
 * key function; its vtable references the inline destructor in daBar_c.h, so
 * the compiler owns retail's D1/D0 pair and the complete RTTI/vtable group.
 *
 * InvisiblePole_Spawn and InvisiblePole_SpawnInfo remain evidence-bounded C
 * ABI aliases. The ROM proves daBar_c's identity, not their original source
 * spelling.
 */

#include "daBar_c.h"

/* Natural new targets the wrong global allocator for actor factories. The
 * measured allocator/base/member construction sequence and the two Fix12-by-
 * value calls therefore remain narrow ABI seams. */
extern "C" {
extern int _ZTV7daBar_c[];
extern void *_ZN7fBase_cnwEj(u32 size);
extern void _ZN8dActor_cC2Ev(dActor_c *actor);
extern void _ZN7dCcAc_cC1Ev(dCcAc_c *clsn);
extern void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(
    dActor_c *actor, Fix12i offsetY, Fix12i radius,
    Fix12i clipDistance, Fix12i farDistance);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    dCcAc_c *clsn, dActor_c *actor, Fix12i radius, Fix12i height,
    u32 flags, u32 vulnFlags);
}

extern "C" daBar_c *InvisiblePole_Spawn();

/* ROM ordinal 7 -- actor-table C ABI factory. */
// @symbol InvisiblePole_Spawn
extern "C" daBar_c *InvisiblePole_Spawn()
{
    daBar_c *bar = (daBar_c *)_ZN7fBase_cnwEj(sizeof(daBar_c));
    if (bar) {
        _ZN8dActor_cC2Ev(bar);
        *(int *)bar = (int)&_ZTV7daBar_c[2];
        _ZN7dCcAc_cC1Ev(&bar->mClsn);
    }
    return bar;
}

extern "C" DaBarSpawnInfo InvisiblePole_SpawnInfo = {
    InvisiblePole_Spawn,
    0x011f,
    0x0099,
    0x00000003,
    0,
    0,
    0,
    0
};

/* ROM ordinal 6 -- key function and class-data owner. */
// @symbol _ZN7daBar_c13InitResourcesEv
s32 daBar_c::InitResources()
{
    s32 height = (((param1 & 0xff) - 0xa) * 0xa) << 0xc;
    if (height <= 0)
        height = 0x1000;
    s32 halfHeight = height >> 1;

    _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(
        this, halfHeight, halfHeight, halfHeight + 0x640000, 0);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mClsn, this, 0x35555, height,
        (param1 & 0x100) ? 0x0280000c : 0x0080000c, 0);
    return 1;
}

/* ROM ordinal 5. */
// @symbol _ZN7daBar_c8BehaviorEv
s32 daBar_c::Behavior()
{
    mClsn.Clear();
    mClsn.Update();
    return 1;
}

/* ROM ordinal 4. */
// @symbol _ZN7daBar_c6RenderEv
s32 daBar_c::Render()
{
    return 1;
}

/* ROM ordinal 3. */
// @symbol _ZN7daBar_c16OnPendingDestroyEv
void daBar_c::OnPendingDestroy()
{
}

/* ROM ordinal 2. */
// @symbol _ZN7daBar_c16CleanupResourcesEv
s32 daBar_c::CleanupResources()
{
    return 1;
}
