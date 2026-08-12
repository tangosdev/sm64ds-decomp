//cpp
// @symbol func_ov006_0210a9a8
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
extern "C" {
extern void _ZN8Particle10SysTrackerD1Ev(void *);
extern void *_ZTV19dScMgSingle3DBase_c[];
extern void *data_020a0eac;
void *func_ov006_0210a9a8(char *c);
void *func_ov006_0210a9a8(char *c) {
    *(void ***)c = data_ov006_0213eaa8;
    func_ov006_020c21e4(c + 0x4f38);
    *(void ***)c = _ZTV19dScMgSingle3DBase_c;
    _ZN8Particle10SysTrackerD1Ev(c + 0x471c);
    _ZN11dScMgBase_cD2Ev(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, *(void **)&data_020a0eac);
    return c;
}
}
