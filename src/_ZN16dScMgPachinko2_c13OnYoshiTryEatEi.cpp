//cpp
// @symbol _ZN16dScMgPachinko2_c13OnYoshiTryEatEi
// recovered name: dScMgPachinko2_c_OnYoshiTryEat_02103cbc
#include "decl_common.h"
#include "dScMgPachinko2_c.h"
/* Slot 18, overriding dScMgBase_c::OnYoshiTryEat(int). That name is a
   placeholder borrowed from dActor_c by slot index. */
extern "C" {
extern void FreeGfxSlotsById(int n);
}

void dScMgPachinko2_c::OnYoshiTryEat(int n)
{
    /* Reading through `ro` keeps mwcc from holding &unk_0bc in a register,
       which costs a word. */
    const dScMgPachinko2_c *ro = this;

    unk_5660 = 0;
    if (n == 0x10) {
        unk_0bc = ro->unk_0bc + 1;
        if (unk_0bc > 0x270e) unk_0bc = 0x270e;
    } else {
        unk_0bc = 0;
        /* dead clamp, kept: dropping it changes the code */
        if (unk_0bc > 0x270e) unk_0bc = 0x270e;
    }
    FreeGfxSlotsById(0x1d);
    func_ov006_02103bfc((char *)this);
}
