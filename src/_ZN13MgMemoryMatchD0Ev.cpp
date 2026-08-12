//cpp
// @symbol func_ov006_020f3888
// recovered name: dScMgMemory_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgMemory_c::OnYoshiTryEat - recovered from vtable slot identity */
extern "C" {
extern void _ZN8Particle10SysTrackerD1Ev(void *);
extern void *data_ov006_0213e448[];
extern void *data_020a0eac;
void *func_ov006_020f3888(char *c);
void *func_ov006_020f3888(char *c) {
    *(void ***)c = data_ov006_0213d1b8;
    func_ov006_020c1c64(c + 0x4f38);
    *(void ***)c = data_ov006_0213e448;
    _ZN8Particle10SysTrackerD1Ev(c + 0x471c);
    _ZN11dScMgBase_cD2Ev(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, *(void **)&data_020a0eac);
    return c;
}
}
