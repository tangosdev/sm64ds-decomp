// @symbol func_ov006_020de5b0
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgCoin_c.h"
// @emits dScMgCoin_c_OnYoshiTryEat_020de5b0
/* recovered: renamed to Class_Method */
/* dScMgCoin_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void FreeGfxSlotsById(int n);
extern int func_ov004_020adc1c(void);
void dScMgCoin_c_OnYoshiTryEat_020de5b0(char *c);
void dScMgCoin_c_OnYoshiTryEat_020de5b0(char *c){
    struct dScMgCoin_c *self = (struct dScMgCoin_c *)(void *)c;
    if (self->unk_51db != 0) {
        *(unsigned char*)(((int)c + 0x51da) & 0xFFFFFFFFFFFFFFFF) += 1;
    } else {
        self->unk_51da = 0;
    }
    self->unk_0a8 = 0;
    self->unk_0ac = self->unk_0a8;
    func_ov006_020ddf9c(c);
    FreeGfxSlotsById(0x1d);
    func_ov006_020dd334(c);
    func_ov006_020dc7b4(c);
    func_ov006_020dc2f8(c);
    self->unk_51d4 = func_ov004_020adc1c();
    self->unk_51c8 = 0;
}
