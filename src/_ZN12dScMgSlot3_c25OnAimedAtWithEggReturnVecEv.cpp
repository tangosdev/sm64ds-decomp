//cpp
// @symbol _ZN12dScMgSlot3_c25OnAimedAtWithEggReturnVecEv
// recovered name: dScMgSlot3_c_OnAimedAtWithEggReturnVec
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgSlot3_c.h"
/* recovered: renamed to Class_Method */
/* dScMgSlot3_c::OnAimedAtWithEggReturnVec - recovered from vtable slot identity.
   The name is dScMgBase_c's, transplanted down the hierarchy; the `ReturnVec`
   half of it is refuted in include/dScMgBase_c.h's slot-30 block, and this
   body is the arity witness cited there.  The read-modify-write below clobbers
   r1 before the tail call, so the base cannot be taking a second argument. */

void dScMgSlot3_c::OnAimedAtWithEggReturnVec()
{
    void *a = (void *)this;

    *(volatile unsigned short*)0x400000A = (*(volatile unsigned short*)0x400000A & 0x43) | 0x1118;
    _ZN11dScMgBase_c25OnAimedAtWithEggReturnVecEv(a);
}
