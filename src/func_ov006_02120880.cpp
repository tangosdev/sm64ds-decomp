//cpp
// @symbol func_ov006_02120880
// @emits dScMgTrampoline_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgTrampoline_c::OnYoshiTryEat - recovered from vtable slot identity */
extern "C" int _ZTV17MgBounceAndPounce;
extern "C" int data_020a0eac;
extern "C" void __destroy_arr(void *p, int a, int b, void (*fn)());
extern "C" void _ZN8Particle10SysTrackerD1Ev(void *p);

extern "C" void *dScMgTrampoline_c_OnYoshiTryEat(char *thiz)
{
    *(int**)thiz = &data_ov006_0213fb34;
    __destroy_arr(thiz + 0x5cd0, 5, 0x24, &func_ov006_02120938);
    __destroy_arr(thiz + 0x534c, 3, 0x32c, &func_ov006_020d1008);
    __destroy_arr(thiz + 0x500c, 4, 0xd0, &func_ov006_020ccfc8);
    *(int**)thiz = &_ZTV17MgBounceAndPounce;
    _ZN8Particle10SysTrackerD1Ev(thiz + 0x47e4);
    func_ov004_020b29c0(thiz);
    _ZN6Memory10DeallocateEPvP4Heap(thiz, *(void**)&data_020a0eac);
    return thiz;
}
