//cpp
// @symbol _ZN11dScMgCoin_c13OnYoshiTryEatEi
#include "decl_common.h"
#include "dScMgCoin_c.h"

/* dScMgCoin_c::OnYoshiTryEat - recovered from ROM RTTI and vtable slot identity.
   void, amended 2026-08-31: recompiled as void this matches byte-identically
   (match.py, 2004/b56), which retires the old `return 0;` -- it was an
   int-assumption transcription of the `mov r0,#0` that is really the source
   register of the unk_51c8 store at the tail, doing double duty only under
   an int reading. Slot 18's family form is void; see dScMgBase_c.h. */
extern "C" {
extern void FreeGfxSlotsById(int n);
extern int func_ov004_020adc1c(void);
}

void dScMgCoin_c::OnYoshiTryEat(int arg)
{
    dScMgCoin_c *self = this;

    if (self->unk_51db != 0) {
        *(unsigned char *)((int)self + 0x51da) += 1;
    } else {
        self->unk_51da = 0;
    }
    self->unk_0a8 = 0;
    *(s32 *)((char *)self + 0xac) = self->unk_0a8;
    func_ov006_020ddf9c((char *)self);
    FreeGfxSlotsById(0x1d);
    func_ov006_020dd334((char *)self);
    func_ov006_020dc7b4((char *)self);
    func_ov006_020dc2f8((char *)self);
    self->unk_51d4 = func_ov004_020adc1c();
    self->unk_51c8 = 0;
}
