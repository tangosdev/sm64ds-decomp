//cpp
/* dActor_c::IsPlayerInRange(const Vector3&, s32) at 0x0201045c, 0x3c bytes.
 *
 * Is the nearest player within maxDist of the position handed in? The overload
 * beside it asks the same question about the actor's own position.
 *
 * A MEMBER WHOSE BODY NEVER TOUCHES A FIELD, and the boundary is what says so:
 * `pos` arrives in r1 (`mov r5, r1`) and maxDist in r2 (`mov r4, r2`), leaving
 * r0 written by every caller and read by nobody here. A static
 * `IsPlayerInRange(const Vector3&, s32)` would have taken `pos` in r0. Same
 * argument as the dust workers, and with the same premise: what this settles is
 * static-vs-non-static GIVEN the mangled name's parameter list. See
 * include/dActor_c.h.
 *
 * maxDist is a whole-unit distance shifted into 20.12 at the comparison
 * (`cmp r0, r4, lsl #12`) -- see the sibling overload.
 */
#include "Player.h"

extern "C" {
/* C linkage -- the ROM spells it unmangled. */
Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);
}

bool dActor_c::IsPlayerInRange(const Vector3 &pos, s32 maxDist)
{
    Player *closest = ClosestPlayer();

    return Vec3_Dist(&pos, (Vector3 *)&closest->mPosX) < (maxDist << 12);
}
