//cpp
// @symbol _ZN14dScMgCurling_c8BehaviorEv
#include "dScMgCurling_c.h"
// recovered name: dScMgCurling_c_Behavior
/* dScMgCurling_c::Behavior - recovered from vtable slot identity. State
   dispatch through a per-index pointer-to-member table, same trick the
   recovered source already used -- kept as a local stand-in cast of
   `this` since the table's own element type isn't evidenced. */
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf[1]; };
extern Entry data_ov006_02141950[];
struct C { char pad[0x4eac]; int idx; };
extern "C" int func_ov006_020e12d0(C*);
s32 dScMgCurling_c::Behavior()
{
    C *c = (C *)this;
    int j = c->idx;
    (c->*data_ov006_02141950[j].pmf[0])();
    func_ov006_020e12d0(c);
    return 1;
}
