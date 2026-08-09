//cpp
// @symbol _ZN6Player23St_InYoshiMouth_CleanupEv
/* recovered: named members + shared header, real C++ method
 *
 * ov002, 0x020d6084, size 0x34. Leaving the state Yoshi has you in: two body
 * collision bits are cleared and two markers set, then it returns 1.
 *
 * `return 1` is not decoration here. ChangeState calls mCleanup FIRST, on the
 * state being left, and TESTS the result -- a 0 vetoes the transition. So this
 * handler's return value is what lets the player leave Yoshi's mouth at all.
 *
 * mBodyClsnFlags at 0x2ec was already in Player.h, named from other evidence,
 * and this function clearing 0x2000 and 0x2 in it is what a cleanup of a
 * carried state looks like. The other two writes are still unnamed offsets --
 * 0x713 and 0x6f5 -- and are left as such rather than guessed at.
 *
 * THE TWO CLEARS DO NOT MERGE. `*p &= ~0x2002` is one instruction shorter and
 * changes the size, so the ROM really does mask twice; measured, not assumed.
 * The `(long long)` round-trip the placeholder body carried was free, and is
 * gone.
 *
 * This file carried the placeholder name func_ov002_020d6084 until the symbol
 * was moved here from ov006. See the commit and include/Player.h.
 */
#include "Player.h"

int Player::St_InYoshiMouth_Cleanup()
{
    unsigned int *p = (unsigned int *)&mBodyClsnFlags;
    *p &= ~0x2000;
    *p &= ~2;
    *(unsigned char *)((char *)this + 0x713) = 1;
    *(unsigned char *)((char *)this + 0x6f5) = 0x1f;
    return 1;
}
