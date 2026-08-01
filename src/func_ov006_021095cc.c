// @symbol func_ov006_021095cc
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgRoulette_c.h"
// recovered name: dScMgRoulette_c_OnYoshiTryEat_021095cc
/* recovered: renamed to Class_Method */
/* dScMgRoulette_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void func_ov004_020b66d4(void);
extern int *func_020beb68;

void func_ov006_021095cc(char *this)
{
    struct dScMgRoulette_c *self = (struct dScMgRoulette_c *)(void *)this;
    int i;
    int *e;
    int n;

    func_ov006_02107b14();
    func_ov006_020c0e8c((int *)(this + 0x4f38));

    self->unk_53f2 = 0;
    self->unk_53e8 = 0xa;

    e = (int *)(this + 0x51a8);
    for (i = 0; i < 5; i++) {
        func_ov006_021092a0(e);
        e = (int *)((char *)e + 0x34);
    }

    self->unk_53f8 = 0;
    self->unk_53f6 = 0;
    n = 0;
    if (func_020beb68 != 0) n = func_020beb68[0xa8 / 4];
    if (n >= 5) n = 5;
    self->unk_53fc = n;

    func_ov006_02108524(this + 0x530c);

    data_ov006_021428c8 = 0;
    self->unk_53e4 = 0;
    self->unk_53e0 = 0x100;
    self->unk_53ea = 0;
    self->unk_53ec = 0;
    self->unk_53ed = 0;
    self->unk_53ee = 0;
    self->unk_53ef = 0;
    self->unk_53f0 = 0;
    self->unk_53e6 = 1;

    func_ov004_020b66d4();
    func_020bc7d4 = 1;
}
