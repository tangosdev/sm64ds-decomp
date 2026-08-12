//cpp
// @symbol func_ov006_020dea1c
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
extern "C" {
extern int __destroy_arr(void*, int, int, void*);
extern void _ZN8Particle10SysTrackerD1Ev(void *);
extern void NullDestructor_0203d47c();
extern void *_ZTV19dScMgSingle3DBase_c[];
extern void *data_020a0eac;
void *func_ov006_020dea1c(char *c);
void *func_ov006_020dea1c(char *c) {
    *(void ***)c = data_ov006_0213c154;
    __destroy_arr(c + 0x53e8, 3, 8, (void*)&NullDestructor_0203d47c);
    __destroy_arr(c + 0x50e8, 0x20, 0x18, (void*)&func_ov006_020deac4);
    func_ov006_020c3288(c + 0x4f38);
    *(void ***)c = _ZTV19dScMgSingle3DBase_c;
    _ZN8Particle10SysTrackerD1Ev(c + 0x471c);
    _ZN11dScMgBase_cD2Ev(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, *(void **)&data_020a0eac);
    return c;
}
}
