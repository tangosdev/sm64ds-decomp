//cpp
// @symbol _ZN13dScMgTeresa_c8BehaviorEv
// recovered name: dScMgTeresa_c_Behavior
/* recovered: renamed to Class_Method */
#include "dScMgTeresa_c.h"
/* dScMgTeresa_c::Behavior - recovered from vtable slot identity */
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern "C" {
extern Entry data_ov006_02142eb0[];
extern int func_ov006_0211e4e0(C* c);
}
s32 dScMgTeresa_c::Behavior()
{
    (((C *)this)->*data_ov006_02142eb0[unk_4be8].pmf)();
    func_ov006_0211e4e0((C *)this);
    return 1;
}
