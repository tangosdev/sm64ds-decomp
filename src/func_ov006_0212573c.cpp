//cpp
// @symbol func_ov006_0212573c
// @emits dScMgSnowball_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgSnowball_c::OnYoshiTryEat - recovered from vtable slot identity */
extern "C" {
extern int func_0207328c(void*, int, int, void*);
extern void _ZN8Particle10SysTrackerD1Ev(void *);
extern void func_0203d47c();
extern void *data_ov006_0213e448[];
extern void *data_020a0eac;
void *dScMgSnowball_c_OnYoshiTryEat(char *c);
void *dScMgSnowball_c_OnYoshiTryEat(char *c) {
    *(void ***)c = data_ov006_0214000c;
    func_0207328c(c + 0xba14, 0x20, 0x24, (void*)&func_ov006_02125800);
    func_0207328c(c + 0xb5d8, 0x80, 8, (void*)&func_0203d47c);
    func_0207328c(c + 0xacd8, 0x80, 8, (void*)&func_0203d47c);
    _ZN5ModelD1Ev(c + 0xaba4);
    *(void ***)c = data_ov006_0213e448;
    _ZN8Particle10SysTrackerD1Ev(c + 0x471c);
    func_ov004_020b29c0(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, *(void **)&data_020a0eac);
    return c;
}
}
