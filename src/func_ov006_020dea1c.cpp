//cpp
// @symbol func_ov006_020dea1c
// @emits dScMgCup_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgCup_c::OnYoshiTryEat - recovered from vtable slot identity */
extern "C" {
extern int __destroy_arr(void*, int, int, void*);
extern void _ZN8Particle10SysTrackerD1Ev(void *);
extern void NullDestructor_0203d47c();
extern void *data_ov006_0213e448[];
extern void *data_020a0eac;
void *dScMgCup_c_OnYoshiTryEat(char *c);
void *dScMgCup_c_OnYoshiTryEat(char *c) {
    *(void ***)c = data_ov006_0213c154;
    __destroy_arr(c + 0x53e8, 3, 8, (void*)&NullDestructor_0203d47c);
    __destroy_arr(c + 0x50e8, 0x20, 0x18, (void*)&func_ov006_020deac4);
    func_ov006_020c3288(c + 0x4f38);
    *(void ***)c = data_ov006_0213e448;
    _ZN8Particle10SysTrackerD1Ev(c + 0x471c);
    func_ov004_020b29c0(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, *(void **)&data_020a0eac);
    return c;
}
}
