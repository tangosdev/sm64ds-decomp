//cpp
// @symbol func_ov006_02107920
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
extern "C" {
extern int __destroy_arr(void*, int, int, void*);
extern void _ZN8Particle10SysTrackerD1Ev(void *);
extern void *_ZTV19dScMgSingle3DBase_c[];
extern void *data_020a0eac;
void *func_ov006_02107920(char *c);
void *func_ov006_02107920(char *c) {
    *(void ***)c = data_ov006_0213e39c;
    _ZN5ModelD1Ev(c + 0x536c);
    _ZN5ModelD1Ev(c + 0x531c);
    __destroy_arr(c + 0x51a8, 5, 0x34, (void*)&func_ov006_021079c8);
    func_ov006_020c1c64(c + 0x4f38);
    *(void ***)c = _ZTV19dScMgSingle3DBase_c;
    _ZN8Particle10SysTrackerD1Ev(c + 0x471c);
    _ZN11dScMgBase_cD2Ev(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, *(void **)&data_020a0eac);
    return c;
}
}
