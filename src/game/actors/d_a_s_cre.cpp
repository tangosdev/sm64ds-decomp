//cpp
/**
 * Star-create trigger.
 *
 * One shot: if the closest player is strictly within 100 units, spawn
 * the star at this position. Then mark this trigger for removal either
 * way.
 *
 * daSCre_c_classInit is reconstructed (RTTI daSCre_c, STAR_CREATE
 * registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: Pos() is a leaf overlay of mPosX/Y/Z. dActor_c::Pos() is
 *   #2513, not this branch.
 */

#include "daSCre_c.h"

enum {
    kPlayerRangeFix12 = 0x64000, /* 100.0 */
    kStarActorId = 0xb2,
    kStarIndexMask = 0xf,
    kStarSpawnStyle = 0x40
};

// @symbol daSCre_c_classInit
extern "C" daSCre_c *daSCre_c_classInit(void)
{
    return new daSCre_c;
}

// @symbol _ZN8daSCre_c8BehaviorEv
s32 daSCre_c::Behavior()
{
    if (DistToCPlayer() < kPlayerRangeFix12) {
        Spawn(kStarActorId, (param1 & kStarIndexMask) | kStarSpawnStyle,
            Pos(), 0, mAreaId, -1); /* no rot, no death-table id */
    }
    MarkForDestruction();
    return 1;
}
