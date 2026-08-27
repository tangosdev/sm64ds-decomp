//cpp
// @symbol func_ov006_021095cc
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgRoulette_c.h"
// recovered name: dScMgRoulette_c_OnYoshiTryEat_021095cc
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
extern "C" void func_ov004_020b66d4(void);
/* data_ov004_020beb68 is declared `void *` by dScMgBase_c.h; cast at the use site. */

extern "C" void func_ov006_021095cc(char *c)
{
    dScMgRoulette_c *self = (dScMgRoulette_c *)(void *)c;
    int i;
    int *e;
    int n;

    func_ov006_02107b14();
    func_ov006_020c0e8c((int *)(c + 0x4f38));

    self->mScore = 0;
    self->mPhaseTimer = 0xa;

    e = (int *)(c + 0x51a8);
    for (i = 0; i < 5; i++) {
        func_ov006_021092a0(e);
        e = (int *)((char *)e + 0x34);
    }

    self->mDealIndex = 0;
    self->mTargetScore = 0;
    n = 0;
    if (data_ov004_020beb68 != 0) n = ((int *)data_ov004_020beb68)[0xa8 / 4];
    if (n >= 5) n = 5;
    self->mRacerCount = n;

    func_ov006_02108524(c + 0x530c);

    data_ov006_021428c8 = 0;
    self->mCameraPreset = 0;
    self->unk_53e0 = 0x100;
    self->unk_53ea = 0;
    self->unk_53ec = 0;
    self->unk_53ed = 0;
    self->unk_53ee = 0;
    self->unk_53ef = 0;
    self->unk_53f0 = 0;
    self->mPhase = 1;

    func_ov004_020b66d4();
    data_ov004_020bc7d4 = 1;
}
