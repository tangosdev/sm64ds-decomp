// @symbol func_ov003_020ad69c
// @emits dScTitle_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScTitle_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void _ZN9ActorBaseD2Ev(void *self);
extern int _ZTV5Stage[];
extern int data_0208e4b8[];
extern void *data_020a0eac;

int *dScTitle_c_OnYoshiTryEat(int *t)
{
    t[0] = (int)data_ov003_020b1650;
    t[0] = (int)_ZTV5Stage;
    t[0] = (int)data_0208e4b8;
    _ZN9ActorBaseD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
