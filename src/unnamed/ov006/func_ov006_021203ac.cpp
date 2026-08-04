//cpp
// @symbol func_ov006_021203ac
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgTeresa_c.h"
// recovered name: dScMgTeresa_c_Behavior
/* recovered: renamed to Class_Method */
/* dScMgTeresa_c::Behavior - recovered from vtable slot identity */
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern Entry data_ov006_02142eb0[];
extern "C" int func_ov006_0211e4e0(C* c);
extern "C" int func_ov006_021203ac(C* c) {
    struct dScMgTeresa_c *self = (struct dScMgTeresa_c *)(void *)c;
    (c->*data_ov006_02142eb0[ self->unk_4be8 ].pmf)();
    func_ov006_0211e4e0(c);
    return 1;
}
