//cpp
#include "types.h"
// @symbol func_ov006_020db9dc
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgCard_c.h"
// recovered name: dScMgCard_c_OnYoshiTryEat_020db9dc
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
extern "C" void func_ov006_020c1604(char *c, int unused, short a2, int a3);
extern "C" void func_ov004_020b66d4(void);
extern int data_ov006_0214176c;
extern int data_ov006_02141768;
extern int data_ov006_02141770;

extern "C" void func_ov006_020db9dc(char *c)
{
    dScMgCard_c *self = (dScMgCard_c *)(void *)c;
    int i;
    char *p1, *p2;

    self->unk_538c = 0;
    self->unk_538e = 6;
    self->unk_5390 = 6;
    self->unk_5392 = 6;
    self->unk_5394 = 6;
    func_ov006_020c1604(c + 0x4f38, 4, 5, (int)(c + 0x538c));

    self->unk_511e = 1;
    self->unk_4f52 = 2;
    func_ov006_020da974(5);

    p1 = c + 0x51a8;
    p2 = c + 0x5298;
    for (i = 0; i < 5; i++) {
        func_ov006_020da00c(p1);
        func_ov006_020da00c(p2);
        p1 += 0x30;
        p2 += 0x30;
    }

    func_ov004_020b66d4();

    func_020bc7d4 = 1;
    data_ov006_0214176c = 0;
    data_ov006_02141768 = 0;
    data_ov006_02141770 = 0;
    self->unk_539a = 0;
    data_ov006_0213bc44 = 1;
    self->unk_5388 = 1;
}
