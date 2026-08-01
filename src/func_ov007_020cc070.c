// @symbol func_ov007_020cc070
// recovered name: dScDSMT_c_OnYoshiTryEat
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: vtable identified, renamed to Class_Method */
/* dScDSMT_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void _ZN9ActorBaseD2Ev(void *);
extern void *G0;
int *func_ov007_020cc070(int *t)
{
    t[0] = (int)VT0;
    func_02017254((char *)t + 0x54);
    t[0] = (int)VT1;
    t[0] = (int)VT2;
    _ZN9ActorBaseD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
