//cpp
#include "dScMgD3DBase_c.h"
// @symbol _ZN14dScMgD3DBase_c24OnHitByCannonBlastedCharEv
// recovered name: dScMgTrampoline2_c_OnHitByCannonBlastedChar  -- WRONG, see below
/* recovered: renamed to Class_Method */
/* dScMgD3DBase_c::OnHitByCannonBlastedChar - slot 26.
 *
 * THE RECOVERED NAME ABOVE NAMES THE WRONG CLASS, and it is kept only so the
 * correction is visible -- the third on this class, after 0x020e6e78 at slot
 * 24 and 0x020e6e54 at slot 25.  This body is at ov006:0x020e6e4c, and word
 * 26 of FIVE vtables points here: _ZTV14dScMgD3DBase_c and all four of its
 * children's -- _ZTV11dScMgJump_c, _ZTV12dScMgJump2_c, _ZTV17dScMgTrampoline_c
 * and _ZTV18dScMgTrampoline2_c.  A body that appears in a class's table AND in
 * every child's is supplied by that class, so it is dScMgD3DBase_c's and the
 * four children inherit it.
 *
 * This also disposes of a guess made while reviewing slot 25: the two earlier
 * misattributions both landed on dScMgJump2_c, which looked like a recovery
 * pass reading each shared body's tables in name order and keeping the last.
 * This one names dScMgTrampoline2_c, which that rule does not produce.
 *
 * The body is `return 2;` where dScMgBase_c's is `return 0;` and
 * dScMgSingle3DBase_c's is `return 1;` -- three distinct constants, which is
 * what pins the return type to int rather than void. */
int dScMgD3DBase_c::OnHitByCannonBlastedChar()
{
    return 2;
}
