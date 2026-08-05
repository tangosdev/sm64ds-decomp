// @symbol func_ov007_020cc070
// recovered name: dScDSMT_c_OnYoshiTryEat
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: vtable identified, renamed to Class_Method */
/* dScDSMT_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void _ZN9ActorBaseD2Ev(void *);
extern void *GAME_HEAP_PTR;
int *func_ov007_020cc070(int *t)
{
    t[0] = (int)_ZTV9dScDSMT_c;
    func_02017254((char *)t + 0x54);
    t[0] = (int)_ZTV5Scene;
    t[0] = (int)_ZTV12ActorDerived;
    _ZN9ActorBaseD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, GAME_HEAP_PTR);
    return t;
}
