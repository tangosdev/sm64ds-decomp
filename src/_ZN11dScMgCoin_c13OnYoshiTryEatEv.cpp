//cpp
// @symbol _ZN11dScMgCoin_c13OnYoshiTryEatEv
#include "decl_common.h"
#include "dScMgCoin_c.h"

/* dScMgCoin_c::OnYoshiTryEat - recovered from ROM RTTI and vtable slot identity. */
extern "C" {
extern void FreeGfxSlotsById(int n);
extern int func_ov004_020adc1c(void);
}

int dScMgCoin_c::OnYoshiTryEat()
{
    dScMgCoin_c *self = this;

    if (self->unk_51db != 0) {
        *(unsigned char *)((int)self + 0x51da) += 1;
    } else {
        self->unk_51da = 0;
    }
    self->unk_0a8 = 0;
    self->unk_0ac = self->unk_0a8;
    func_ov006_020ddf9c((char *)self);
    FreeGfxSlotsById(0x1d);
    func_ov006_020dd334((char *)self);
    func_ov006_020dc7b4((char *)self);
    func_ov006_020dc2f8((char *)self);
    self->unk_51d4 = func_ov004_020adc1c();
    self->unk_51c8 = 0;
    return 0;
}
