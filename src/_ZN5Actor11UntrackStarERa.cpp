//cpp
// @symbol _ZN5Actor11UntrackStarERa
/* Actor::UntrackStar(s8 &starID) at 0x0200ff60.
 *
 * Releases the star-marker slot named by starID and writes -1 back through the
 * reference so the caller's field records "no slot". A negative value already
 * means that, so the function is a no-op in that case -- the early exit is a
 * conditional return (`poplt`/`bxlt`), not a branch over the body.
 *
 * The referent is signed: the ROM opens `ldrsb r0, [r4]`, and the same r0 is
 * handed straight to SetStarMarker without a reload, which is why the slot
 * argument needs no conversion here either.
 */
#include "Actor.h"

extern "C" void SetStarMarker(s32 slot, Actor *actor, s32 markerType);

void Actor::UntrackStar(s8 &starID)
{
    s8 slot = starID;
    if (slot < 0)
        return;

    SetStarMarker(slot, 0, 2);
    starID = -1;
}
