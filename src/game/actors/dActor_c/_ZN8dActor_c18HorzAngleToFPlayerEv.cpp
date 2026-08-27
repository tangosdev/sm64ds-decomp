//cpp
/* dActor_c::HorzAngleToFPlayer() at 0x0201092c, 0x2c bytes.
 *
 * The horizontal angle from this actor to the farthest player.
 *
 * The discarded ClosestPlayer() result is the point, not an oversight: that
 * call is what refills data_0209b450, and the global is read immediately after.
 * Calling it for the side effect and then reading the cache is the shape of
 * every reader in this group -- see include/dActor_c.h.
 *
 * Note it still calls CLOSESTPlayer: one walk of the player table caches
 * both ends, so the farthest pointer is a by-product of finding the nearest.
 *
 * A member: `mov r4, r0` then `add r0, r4, #0x5c` hands Vec3_HorzAngle this
 * actor's own position. Returns s16 -- it tails straight into Vec3_HorzAngle.
 *
 * The old file reached 0x5c through a shadow `struct dActor_c { char _pad[0x5c];
 * Vector3 pos; }`; mPosX has been named in the real header all along.
 */
#include "dActor_c.h"

extern "C" {
/* C linkage -- the ROM spells both unmangled. Taken by pointer, which is how
   every other caller in the tree declares them. */
s16    Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);

/* Refilled by ClosestPlayer(); see the proximity note in include/dActor_c.h.
   Declared dActor_c* rather than void* so the 0x5c read is a field access. */
extern dActor_c *data_0209b458;   /* nearest player  */
extern dActor_c *data_0209b450;   /* farthest player */
}

s16 dActor_c::HorzAngleToFPlayer()
{
    ClosestPlayer();
    return Vec3_HorzAngle((Vector3 *)&mPosX, (Vector3 *)&data_0209b450->mPosX);
}
