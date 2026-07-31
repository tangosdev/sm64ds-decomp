//cpp
// @symbol func_ov006_0210a9a8
// @emits dScMgSlot3_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgSlot3_c::OnYoshiTryEat - recovered from vtable slot identity */
extern "C" {
extern void _ZN8Particle10SysTrackerD1Ev(void *);
extern void *data_ov006_0213e448[];
extern void *data_020a0eac;
void *dScMgSlot3_c_OnYoshiTryEat(char *c);
void *dScMgSlot3_c_OnYoshiTryEat(char *c) {
    *(void ***)c = data_ov006_0213eaa8;
    func_ov006_020c21e4(c + 0x4f38);
    *(void ***)c = data_ov006_0213e448;
    _ZN8Particle10SysTrackerD1Ev(c + 0x471c);
    func_ov004_020b29c0(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, *(void **)&data_020a0eac);
    return c;
}
}
