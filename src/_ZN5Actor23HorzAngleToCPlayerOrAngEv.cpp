//cpp
/* Actor::HorzAngleToCPlayerOrAng() at 0x0201097c, 0x3c bytes.
 *
 * HorzAngleToCPlayer with a fallback: if there is no nearest player, face the
 * way you already are. The "OrAng" in the name is that fallback -- the actor's
 * own facing angle, mAngleY.
 *
 * This is the one member of the group that proves its own return width. The
 * fallback path is `ldrsheq r0, [r4, #0x8e]`, a SIGNED halfword load of
 * mAngleY at 0x08e, so the function returns s16 and the field is signed. The
 * other two readers only tail into Vec3_HorzAngle and inherit its width.
 *
 * The null check is on the CACHE, not on ClosestPlayer's return value: the ROM
 * loads 0x0209b458 after the call and compares that. Both are the same pointer
 * in practice, but the bytes read the global, so this does too.
 *
 * The old file used a shadow `struct Actor` with a `Vector3_16 ang` at 0x8c to
 * reach the angle; mAngleX/Y/Z have been named in the real header all along.
 */
#include "Actor.h"

extern "C" {
/* C linkage -- the ROM spells it unmangled. */
s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);

/* Refilled by ClosestPlayer(); see the proximity note in include/Actor.h. */
extern Actor *data_0209b458;
}

s16 Actor::HorzAngleToCPlayerOrAng()
{
    Actor *player;

    ClosestPlayer();
    player = data_0209b458;
    if (player == 0)
        return mAngleY;

    return Vec3_HorzAngle((Vector3 *)&mPosX, (Vector3 *)&player->mPosX);
}
