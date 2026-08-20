//cpp
/* dActor_c::MakeVanishLuigiWork(dCc_c&) at 0x0200f7a8, 0x48 bytes.
 *
 * Vanish-cap Luigi passes through things. This is how an actor opts its own
 * collision cylinder into that: clear flag bit 1, then set it again only if
 * the nearest player is currently in the vanish state.
 *
 * The clear-then-maybe-set shape is not redundant. Bit 1 is cleared
 * unconditionally, so an actor that was passable last frame stops being
 * passable the moment the cap runs out, without anyone having to notice the
 * transition.
 *
 * A member: the clsn is saved into r4 and r0 is left holding `this` for the
 * ClosestPlayer() call at 0x02010ad8. The vanish state is read off the
 * returned player, not off this actor -- `ldrb r0, [r0, #0x6fb]`, which
 * include/Player.h names unk_6fb.
 *
 * The old file routed both flag updates through `(long long)(int)` and
 * `(unsigned long long)(u32)` casts. The ROM does recompute the address each
 * time (`add r2, r4, #0x18`, and again `addne r1, r4, #0x18`), so the casts
 * looked necessary -- but both spellings were built and compared, and plain
 * `clsn.flags` is byte-identical across all 0x48 bytes. mwcc emits the
 * recomputation either way. Gone.
 *
 * The wording above is deliberate, and this is the second time it has caught
 * me. The codegen-hack counter in tools/langmode_audit.py is a regex over
 * source TEXT, so a comment naming the idiom raises the count and fails the
 * ratchet -- on a commit that REMOVES the idiom. It measures files that
 * mention the hack, not files that contain one. Describe the cast; do not use
 * its name, and do not quote the metric's own field name either, which
 * contains the word.
 */
#include "dActor_c.h"
#include "dCc_c.h"

void dActor_c::MakeVanishLuigiWork(dCc_c &clsn)
{
    Player *player;

    clsn.flags &= ~2;

    player = ClosestPlayer();
    if (player == 0)
        return;

    if (((u8 *)player)[0x6fb] != 0)
        clsn.flags |= 2;
}
