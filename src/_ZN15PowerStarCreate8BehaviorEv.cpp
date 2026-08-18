//cpp
// @symbol _ZN15PowerStarCreate8BehaviorEv
// recovered name: PowerStarCreate::Behavior
/* PowerStarCreate::Behavior - vtable slot 6, overriding fBase_c::Behavior().
 * Migrated to a real member: every field this touches (param1 at 0x008,
 * mPosX/mPosY/mPosZ at 0x05c, mAreaId at 0x0cc) is dActor_c's own, already
 * recovered in include/dActor_c.h -- PowerStarCreate itself adds no typed
 * fields (include/PowerStarCreate.h: only pad_0d0[0x4]). dActor_c::Spawn
 * and dActor_c::DistToCPlayer/fBase_c::MarkForDestruction are real
 * (non-virtual) members there. */
#include "PowerStarCreate.h"

s32 PowerStarCreate::Behavior()
{
    if (DistToCPlayer() < 0x64000) {
        Spawn(0xb2, (param1 & 0xf) | 0x40, *(Vector3 *)&mPosX, 0, mAreaId, -1);
    }
    MarkForDestruction();
    return 1;
}
