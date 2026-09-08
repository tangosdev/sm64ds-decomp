//cpp
/**
 * Star-create trigger.
 *
 * When Mario is close enough it spawns the star at this position and
 * deletes itself.
 *
 * daSCre_c_classInit is reconstructed (RTTI daSCre_c, STAR_CREATE
 * registry). Retail does not store that spelling.
 *
 * deslop
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
