// @symbol func_ov003_020b05bc
// recovered name: dScGameOver_c_OnYoshiTryEat
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: vtable identified, renamed to Class_Method */
/* dScGameOver_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void _ZN9ActorBaseD2Ev(void *);
extern void *GAME_HEAP_PTR;
int *func_ov003_020b05bc(int *t)
{
    t[0] = (int)_ZTV13dScGameOver_c;
    t[0] = (int)_ZTV5Scene;
    t[0] = (int)_ZTV12ActorDerived;
    _ZN9ActorBaseD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, GAME_HEAP_PTR);
    return t;
}
