//cpp
// @symbol _ZN15dScMgPachinko_c13OnYoshiTryEatEi
// recovered name: dScMgPachinko_c_OnYoshiTryEat_020fed58
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgPachinko_c.h"
/* recovered: renamed to Class_Method, vtable slot 18 -- an override of
   dScMgBase_c::OnYoshiTryEat(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
void dScMgPachinko_c::OnYoshiTryEat(int n)
{
    /* The score increment reads unk_0bc through a const view of `this`.
       Without it mwcc CSEs the +0xbc field address into its own register
       (add r2,r4,#0xbc / ldr [r2] / str [r2]) and the function grows a
       word; the cartridge re-issues ldr r1,[r4,#0xbc] / str r1,[r4,#0xbc].
       Same lever as dScMgPachinko2_c::OnYoshiTryEat. */
    const dScMgPachinko_c *ro = this;

    unk_5c10 = 0;
    if (n == 9) {
        unk_0bc = ro->unk_0bc + 1;
        if (unk_0bc > 0x270e) unk_0bc = 0x270e;
    } else {
        unk_0bc = 0;
        if (unk_0bc > 0x270e) unk_0bc = 0x270e;
    }
    func_ov006_020fadfc((char *)this);
    func_ov006_020fad90((char *)this);
}
