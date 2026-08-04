// @symbol _ZN11SoundObjectD0Ev
// recovered name: daSoundObj_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daSoundObj_c::OnYoshiTryEat - recovered from vtable slot identity */
int *_ZN11SoundObjectD0Ev(int *t)
{
    t[0] = (int)VT;
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, HEAP);
    return t;
}
