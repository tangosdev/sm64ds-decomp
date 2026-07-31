//cpp
// @symbol func_ov006_020f8f68
// @emits dScMgMCarlo2_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgMCarlo2_c::OnYoshiTryEat - recovered from vtable slot identity */
extern "C" {
extern int func_0207328c(void*, int, int, void*);
extern void _ZN8Particle10SysTrackerD1Ev(void *);
extern void *data_ov006_0213e448[];
extern void *data_020a0eac;
void *dScMgMCarlo2_c_OnYoshiTryEat(char *c);
void *dScMgMCarlo2_c_OnYoshiTryEat(char *c) {
    *(void ***)c = data_ov006_0213d7e8;
    func_0207328c(c + 0x51a8, 0x28, 0x30, (void*)&func_ov006_020f8ff0);
    func_ov006_020c1c64(c + 0x4f38);
    *(void ***)c = data_ov006_0213e448;
    _ZN8Particle10SysTrackerD1Ev(c + 0x471c);
    func_ov004_020b29c0(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, *(void **)&data_020a0eac);
    return c;
}
}
