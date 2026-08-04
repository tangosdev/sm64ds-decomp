// @symbol func_ov005_020bfefc
// recovered name: dScMiniGm_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMiniGm_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void _ZN9ActorBaseD2Ev(void *self);
extern void*_ZTV5Scene[];
extern int data_0208e4b8[];
extern void *data_020a0eac;

int *func_ov005_020bfefc(int *t)
{
    t[0] = (int)data_ov005_020c2490;
    t[0] = (int)_ZTV5Scene;
    t[0] = (int)data_0208e4b8;
    _ZN9ActorBaseD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
