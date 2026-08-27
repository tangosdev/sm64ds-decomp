//cpp
// @symbol _ZN12dScMgLuigi_c8BehaviorEv
// recovered name: dScMgLuigi_c_Behavior
/* recovered: renamed to Class_Method */
#include "dScMgLuigi_c.h"
/* dScMgLuigi_c::Behavior - recovered from vtable slot identity */
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern "C" {
extern Entry data_ov006_02142234[];
}
s32 dScMgLuigi_c::Behavior()
{
    struct D { char pad[0x4f78]; int idx; };
    D *c = (D *)this;
    int j = c->idx;
    (((C *)c)->*data_ov006_02142234[j].pmf)();
    return 1;
}
