//cpp
// @symbol _ZN12dScMgSlot1_c19OnHitFromUnderneathER8dActor_c
/* dScMgSlot1_c::OnHitFromUnderneath -- slot 28, signature copied from
   include/dActor_c.h's own slot 28 (see include/dScMgSlot1_c.h's file banner
   for why that's the right signature even though nothing in this
   hierarchy branch declares it yet). Calls straight into dScMgBase_c's own
   OnHitFromUnderneath (func_ov004_020af04c, itself unmigrated). The
   `other` parameter is unused -- same as dActor_c::OnHitFromUnderneath's own
   trivial stub (src/_ZN8dActor_c19OnHitFromUnderneathERS_.cpp): the ROM sets
   no r0 here either, so the declared `int` return and the pre-migration
   `void` return compile to the same bytes. */
#include "decl_common.h"
#include "dScMgSlot1_c.h"

extern "C" void func_ov004_020af04c(void* self);

int dScMgSlot1_c::OnHitFromUnderneath(dActor_c &other)
{
    func_ov004_020af04c(this);
    SetSubBg1Offset(0x100, 0);
}
