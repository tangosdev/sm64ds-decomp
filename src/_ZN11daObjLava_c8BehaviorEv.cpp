//cpp
// @symbol _ZN11daObjLava_c8BehaviorEv
/* daObjLava_c::Behavior -- vtable slot 6.
 *
 * Every frame it re-issues particle effect 0xb7 -- the lava bubble -- at the
 * closest player's position, feeding last frame's handle back in so the system
 * recycles the same emitter instead of stacking new ones. */
#include "daObjLava_c.h"
#include "Player.h"
#include "types.h"

extern "C" u32 func_02022c3c(u32 handle, u32 effectID,
                             Fix12i x, Fix12i y, Fix12i z, const void *dir);

s32 daObjLava_c::Behavior()
{
    /* The ROM forms &player->mPosX once and loads all three words off it, which
     * is what taking the position as a Vector3 does; three separate member
     * loads off the Player pointer keep the 0x5c/0x60/0x64 offsets instead. */
    Vector3 *pos = (Vector3 *)&ClosestPlayer()->mPosX;
    mEffectHandle = func_02022c3c(mEffectHandle, 0xb7, pos->x, pos->y, pos->z, 0);
    return 1;
}
