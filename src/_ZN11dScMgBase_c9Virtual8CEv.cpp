//cpp
// @symbol _ZN11dScMgBase_c9Virtual8CEv
/* dScMgBase_c::Virtual8C - slot 35, the LAST slot of dScMgBase_c's own
   eighteen.  Twenty bytes, and the end of the 18-35 keystone range: with this
   declared the class emits its full 36-slot vtable from source and no table in
   the minigame family is a prefix any more.

   It is a predicate on the scene's own spawn parameter: `param1 & 0xff`, the
   fBase_c word at +0x08 that every fBase_c is constructed with, tested against
   zero.  dScMgAmida_c is the only class in the family that overrides it, and
   it asks the narrower question (`== 1`); the other 31 tables carry this body.

   Thirteen call sites, all in ov006, all the same shape --
   `mov r0,<this>; ldr r1,[r0]; ldr r1,[r1,#0x8c]; blx r1` -- spread across
   FOUR leaf classes' code regions: dScMgCoin_c (2), dScMgPanel_c (4),
   dScMgSound_c (3), dScMgSnowball_c (4).  Each is a class asking the question
   of itself and branching on the answer, which is how a single minigame runs
   in two variants: two asset tables at ov006:0x02105488, a different field
   path at 0x0211b9e0, a whole block skipped at 0x02126f58.

   All thirteen follow the call with `cmp r0, #0`.  That makes this the one
   slot in the campaign whose return value is not merely permitted by the ROM
   but seen to be consumed by it.

   `Virtual8C` is a placeholder after the +0x8c vtable offset, not a ROM name;
   as at slots 33 and 34 there was no `recovered name:` line here to correct.

   The rename had to be scoped: 0x020ad660 is an overlay load base, so ov000,
   ov002, ov003, ov004 and ov007 all have unrelated symbols at that address
   (ov003's is dScTitle_c's D1).  Only ov004's is this function. */
#include "dScMgBase_c.h"

int dScMgBase_c::Virtual8C()
{
    return (param1 & 0xff) != 0;
}
