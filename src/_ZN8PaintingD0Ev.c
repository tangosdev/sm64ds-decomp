// @symbol func_ov080_02125428
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_common.h"
extern void* data_020a0eac;
extern int data_ov080_021282b4[];
/* recovered: renamed to Class_Method */
int *func_ov080_02125428(int *t)
{
    t[0] = (int)data_ov080_021282b4;
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
