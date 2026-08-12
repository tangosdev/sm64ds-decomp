//cpp
// @symbol _ZN12dScMgSlot1_c15OnHitByMegaCharER6Player
/* dScMgSlot1_c::OnHitByMegaChar -- slot 27, signature copied from
   include/Actor.h's own slot 27 (see include/dScMgSlot1_c.h's file banner
   for why that's the right signature even though nothing in this
   hierarchy branch declares it yet). Calls straight into dScMgBase_c's own
   OnHitByMegaChar (func_ov004_020af27c, itself unmigrated). The `player`
   parameter is unused -- same as Actor::OnHitByMegaChar's own trivial stub
   (src/_ZN5Actor15OnHitByMegaCharER6Player.cpp): the ROM sets no r0 here
   either, so the declared `int` return and the pre-migration `void` return
   compile to the same bytes. */
#include "decl_common.h"
#include "dScMgSlot1_c.h"

extern "C" void func_ov004_020af27c(void* c);

int dScMgSlot1_c::OnHitByMegaChar(Player &player)
{
    SetSubBg1Offset(0, 0);
    func_ov004_020af27c(this);
}
