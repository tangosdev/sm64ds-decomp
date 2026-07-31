//cpp
// @symbol func_ov006_020eec9c
// @emits dScMgJump2_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Model.h"
#include "decl_Player.h"
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgJump2_c::OnYoshiTryEat - recovered from vtable slot identity */
extern "C" {
extern int __destroy_arr(void*, int, int, void*);
extern void _ZN8Particle10SysTrackerD1Ev(void *);
extern void *_ZTV17MgBounceAndPounce[];
extern void *data_020a0eac;
void *dScMgJump2_c_OnYoshiTryEat(char *c);
void *dScMgJump2_c_OnYoshiTryEat(char *c) {
    *(void ***)c = data_ov006_0213ccfc;
    _ZN5ModelD1Ev(c + 0x5a14);
    __destroy_arr(c + 0x57d4, 0x10, 0x24, (void*)&func_ov006_020eed64);
    __destroy_arr(c + 0x5234, 6, 0xf0, (void*)&_ZN6Player18St_LevelEnter_MainEv);
    __destroy_arr(c + 0x500c, 3, 0xb8, (void*)&func_ov006_020c893c);
    *(void ***)c = _ZTV17MgBounceAndPounce;
    _ZN8Particle10SysTrackerD1Ev(c + 0x47e4);
    func_ov004_020b29c0(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, *(void **)&data_020a0eac);
    return c;
}
}
