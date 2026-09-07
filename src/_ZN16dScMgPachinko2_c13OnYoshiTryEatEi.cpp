//cpp
// @symbol _ZN16dScMgPachinko2_c13OnYoshiTryEatEi
// recovered name: dScMgPachinko2_c_OnYoshiTryEat_02103cbc
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgPachinko2_c.h"
/* recovered: renamed to Class_Method, vtable slot 18 -- an override of
   dScMgBase_c::OnYoshiTryEat(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
extern "C" {
extern void FreeGfxSlotsById(int n);
extern void func_ov006_02103bfc(char *c);
}

void dScMgPachinko2_c::OnYoshiTryEat(int n)
{
    /* The score increment reads unk_0bc through a const view of `this`.
       Without it mwcc CSEs the +0xbc field address into its own register
       (add r2,r4,#0xbc / ldr [r2] / str [r2]) and the function grows a
       word; the cartridge re-issues ldr r1,[r4,#0xbc] / str r1,[r4,#0xbc].
       Same lever as dScMgHanachan_c::OnYoshiTryEat. */
    const dScMgPachinko2_c *ro = this;

    unk_5660 = 0;
    if (n == 0x10) {
        unk_0bc = ro->unk_0bc + 1;
        if (unk_0bc > 0x270e) unk_0bc = 0x270e;
    } else {
        unk_0bc = 0;
        if (unk_0bc > 0x270e) unk_0bc = 0x270e;
    }
    FreeGfxSlotsById(0x1d);
    func_ov006_02103bfc((char *)this);
}
