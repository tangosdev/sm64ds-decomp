//cpp
#include "dScMgD3DBase_c.h"
// @symbol _ZN14dScMgD3DBase_c15OnHitByMegaCharEv
// recovered name: dScMgTrampoline2_c_OnHitByMegaChar  -- WRONG, see below
/* recovered: renamed to Class_Method */
/* dScMgD3DBase_c::OnHitByMegaChar - slot 27.
 *
 * THE RECOVERED NAME ABOVE NAMES THE WRONG CLASS, kept so the correction stays
 * visible.  It is the fourth on this class -- after 0x020e6e78 at slot 24,
 * 0x020e6e54 at slot 25 and 0x020e6e4c at slot 26 -- and the fifth in the
 * campaign.  Same argument as those three: word 27 of FIVE vtables points at
 * this body, _ZTV14dScMgD3DBase_c and all four of its children's, and a body
 * that appears in a class's table AND in every child's is supplied by that
 * class.
 *
 * AND THE TWELVE BYTES ARE NOT A LINKER ARTIFACT, which is worth stating here
 * because they look exactly like one: ldr ip, [pc]; bx ip; .word -- a
 * long-branch veneer from ov006 into ov004, and a forwarding override whose
 * one-instruction tail call is all mwldarm has to work with compiles to
 * precisely this.  The tell that it is a real function symbol rather than a
 * stub the linker interposed is in the OTHER twenty-six tables: the classes
 * that do not override slot 27 hold 0x020af27c, dScMgBase_c's body, directly.
 * Vtable words are data, not branches, so nothing in them needs veneering.  A
 * veneer appears here only because a real function here calls across the
 * overlay boundary. */
extern "C" {
extern void _ZN11dScMgBase_c15OnHitByMegaCharEv(void);
}

void dScMgD3DBase_c::OnHitByMegaChar()
{
    _ZN11dScMgBase_c15OnHitByMegaCharEv();
}
