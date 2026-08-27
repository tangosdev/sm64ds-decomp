//cpp
// @symbol _ZN6Player17St_Cannon_CleanupEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
// Player::St_Cannon_Cleanup - clear flag bit 0x20 in the u32 at this+0x2ec, return 1.
// The u64-mask launder forces the base to materialize (add r2,this,#0x2ec) so the
// load/store reuse it, matching the ROM instead of folding the offset into ldr/str.

int Player::St_Cannon_Cleanup()
{
    *(unsigned int *)((char *)&mdCcAcPos_c.flags) &= ~0x20;
    return 1;
}
