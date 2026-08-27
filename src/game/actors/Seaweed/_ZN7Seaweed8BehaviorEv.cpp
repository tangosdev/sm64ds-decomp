//cpp
// @symbol _ZN7Seaweed8BehaviorEv
#include "Seaweed.h"
// recovered name: daObjWakame_c_Behavior
/* recovered: renamed to Class_Method */
/* daObjWakame_c::Behavior - recovered from vtable slot identity */
extern "C" {
extern void _ZN9Animation7AdvanceEv(void *);
}

s32 Seaweed::Behavior() {
    void * t = (void *)this;
    _ZN9Animation7AdvanceEv((char *)t + 0x124);
    return 1;
}
