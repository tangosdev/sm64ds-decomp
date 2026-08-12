//cpp
// @symbol func_ov006_020d9638
// recovered name: dScMgCard_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgCard_c::OnYoshiTryEat - recovered from vtable slot identity */
extern "C" {
extern int __destroy_arr(void*, int, int, void*);
extern void _ZN8Particle10SysTrackerD1Ev(void *);
extern void *_ZTV19dScMgSingle3DBase_c[];
extern void *data_020a0eac;
void *func_ov006_020d9638(char *c);
void *func_ov006_020d9638(char *c) {
    *(void ***)c = data_ov006_0213bdb4;
    __destroy_arr(c + 0x5298, 5, 0x30, (void*)&func_ov006_020d96f0);
    __destroy_arr(c + 0x51a8, 5, 0x30, (void*)&func_ov006_020d96e0);
    func_ov006_020c1c64(c + 0x4f38);
    *(void ***)c = _ZTV19dScMgSingle3DBase_c;
    _ZN8Particle10SysTrackerD1Ev(c + 0x471c);
    _ZN11dScMgBase_cD2Ev(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, *(void **)&data_020a0eac);
    return c;
}
}
