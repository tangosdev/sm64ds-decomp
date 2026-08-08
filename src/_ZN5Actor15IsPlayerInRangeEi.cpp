//cpp
/* Actor::IsPlayerInRange(s32) at 0x020104dc, 0x3c bytes.
 *
 * Is the nearest player within maxDist of this actor?
 *
 * Unlike the angle readers beside it, this one uses ClosestPlayer's RETURN
 * value rather than the cached global -- `mov r1, r0` immediately after the
 * `bl`. Same pointer either way, but the bytes read the return, so this does.
 *
 * maxDist is a WHOLE-UNIT distance, not a fix12 one. The comparison is
 * `cmp r0, r4, lsl #12`: the caller's plain integer is shifted into 20.12 at
 * the point of comparison, so `IsPlayerInRange(20)` means twenty units, and
 * passing a fix12 value here would ask about 4096 times too far.
 *
 * A member: `mov r5, r0` then `add r0, r5, #0x5c` measures from this actor's
 * own position. The old file reached that through a shadow
 * `struct Actor { char _pad[0x5c]; Vector3 pos; }`.
 */
#include "Player.h"

extern "C" {
/* C linkage -- the ROM spells it unmangled. */
Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);
}

bool Actor::IsPlayerInRange(s32 maxDist)
{
    Player *closest = ClosestPlayer();

    return Vec3_Dist((Vector3 *)&mPosX, (Vector3 *)&closest->mPosX) < (maxDist << 12);
}
