//cpp
// @symbol _ZN12dScMgSlot3_c16OnAimedAtWithEggEv
// recovered name: dScMgSlot3_c_OnAimedAtWithEgg
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgSlot3_c.h"
/* dScMgSlot3_c::OnAimedAtWithEgg - recovered from vtable slot identity.

   This body is the third and strongest witness that slot 29 takes no
   explicit parameter.  It CLOBBERS r1 -- the masked read-modify-write of the
   sub display-control register below compiles to `ldrh r1,[r2]; and; orr;
   strh` -- and only then tail-branches into the base with `bx ip`.  A second
   argument passed in r1 would reach dScMgBase_c::OnAimedAtWithEgg as a
   display-control word, so the base cannot be reading one. */

int dScMgSlot3_c::OnAimedAtWithEgg()
{
    void *a = (void *)this;

    *(volatile unsigned short*)0x400000A = (*(volatile unsigned short*)0x400000A & 0x43) | 0x1000;
    _ZN11dScMgBase_c16OnAimedAtWithEggEv(a);
}
