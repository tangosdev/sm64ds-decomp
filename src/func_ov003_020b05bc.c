// @symbol func_ov003_020b05bc
// @emits dScGameOver_c_OnYoshiTryEat
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: vtable identified, renamed to Class_Method */
/* dScGameOver_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void _ZN9ActorBaseD2Ev(void *);
extern void *G0;
int *dScGameOver_c_OnYoshiTryEat(int *t)
{
    t[0] = (int)VT0;
    t[0] = (int)VT1;
    t[0] = (int)VT2;
    _ZN9ActorBaseD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
