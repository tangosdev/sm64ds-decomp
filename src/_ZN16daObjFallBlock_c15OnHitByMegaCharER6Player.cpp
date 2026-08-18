//cpp
// @symbol _ZN16daObjFallBlock_c15OnHitByMegaCharER6Player
#include "daObjFallBlock_c.h"
/* daObjFallBlock_c::OnHitByMegaChar - the draft cast `this` to a 32-entry
   vtable-shim struct and called its last slot (31); that slot is Kill(),
   already a named virtual on this class, so this calls it directly. The
   `player` parameter is never read, matching the ROM body, which only
   ever takes the one (`this`) argument. */
void daObjFallBlock_c::OnHitByMegaChar(Player &player)
{
    if (mRespawnDelay != 0) return;
    Kill();
    mRespawnDelay = 0xa;
    mState = 3;
    mStateTimer = 0x3c;
}
