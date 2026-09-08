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
 * Leftover: *(Vector3 *)&mPosX (dActor stores x/y/z as three s32s, not
 * a Vector3 field).
 */

#include "daSCre_c.h"

enum {
    kPlayerRangeFix12 = 0x64000, /* 100.0 */
    kStarActorId = 0xb2,
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
        Spawn(kStarActorId, (param1 & 0xf) | kStarSpawnStyle,
            *(Vector3 *)&mPosX, 0, mAreaId, -1);
    }
    MarkForDestruction();
    return 1;
}
