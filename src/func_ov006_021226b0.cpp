//cpp
// @symbol func_ov006_021226b0
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
extern "C" int _ZTV17MgBounceAndPounce;
extern "C" int data_020a0eac;
extern "C" void __destroy_arr(void *p, int a, int b, void (*fn)());
extern "C" void _ZN8Particle10SysTrackerD1Ev(void *p);

extern "C" void *func_ov006_021226b0(char *thiz)
{
    *(int**)thiz = &data_ov006_0213fc7c;
    __destroy_arr(thiz + 0x7ad0, 5, 0x24, &func_ov006_02120938);
    __destroy_arr(thiz + 0x7164, 0x14, 0x78, &func_ov006_02122c68);
    __destroy_arr(thiz + 0x6ffc, 0xa, 0x24, &func_ov006_020eed64);
    __destroy_arr(thiz + 0x5ddc, 0xa, 0x1d0, &func_ov006_021227c8);
    __destroy_arr(thiz + 0x5458, 3, 0x32c, &func_ov006_020d1008);
    __destroy_arr(thiz + 0x500c, 5, 0xdc, &func_ov006_020ca604);
    *(int**)thiz = &_ZTV17MgBounceAndPounce;
    _ZN8Particle10SysTrackerD1Ev(thiz + 0x47e4);
    _ZN11dScMgBase_cD2Ev(thiz);
    _ZN6Memory10DeallocateEPvP4Heap(thiz, *(void**)&data_020a0eac);
    return thiz;
}
