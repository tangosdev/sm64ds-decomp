//cpp
// @symbol func_ov006_0212a5c8
// @emits dScMgFlower_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgFlower_c::OnYoshiTryEat - recovered from vtable slot identity */
extern "C" {
extern int func_0207328c(void*, int, int, void*);
extern void _ZN8Particle10SysTrackerD1Ev(void *);
extern void *data_ov006_0213e448[];
extern void *data_020a0eac;
void *dScMgFlower_c_OnYoshiTryEat(char *c);
void *dScMgFlower_c_OnYoshiTryEat(char *c) {
    *(void ***)c = data_ov006_02140140;
    func_ov006_020c3e70(c + 0x51f8);
    func_0207328c(c + 0x4f38, 0x16, 0x20, (void*)&func_ov006_0212a650);
    *(void ***)c = data_ov006_0213e448;
    _ZN8Particle10SysTrackerD1Ev(c + 0x471c);
    func_ov004_020b29c0(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, *(void **)&data_020a0eac);
    return c;
}
}
