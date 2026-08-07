//cpp
// @symbol func_ov006_0212573c
// recovered name: dScMgSnowball_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgSnowball_c::OnYoshiTryEat - recovered from vtable slot identity */
extern "C" {
extern int __destroy_arr(void*, int, int, void*);
extern void _ZN8Particle10SysTrackerD1Ev(void *);
extern void NullDestructor_0203d47c();
extern void *data_ov006_0213e448[];
extern void *data_020a0eac;
void *func_ov006_0212573c(char *c);
void *func_ov006_0212573c(char *c) {
    *(void ***)c = data_ov006_0214000c;
    __destroy_arr(c + 0xba14, 0x20, 0x24, (void*)&func_ov006_02125800);
    __destroy_arr(c + 0xb5d8, 0x80, 8, (void*)&NullDestructor_0203d47c);
    __destroy_arr(c + 0xacd8, 0x80, 8, (void*)&NullDestructor_0203d47c);
    _ZN5ModelD1Ev(c + 0xaba4);
    *(void ***)c = data_ov006_0213e448;
    _ZN8Particle10SysTrackerD1Ev(c + 0x471c);
    _ZN11dScMgBase_cD2Ev(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, *(void **)&data_020a0eac);
    return c;
}
}
