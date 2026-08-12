// @symbol _ZN4DoorD0Ev
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ModelAnim.h"
#include "decl_common.h"
extern int _ZTV4Door[];
/* recovered: vtable identified, renamed to Class_Method */
extern void *data_020a0eac;
int *_ZN4DoorD0Ev(int *t)
{
    t[0] = (int)_ZTV4Door;
    _ZN9ModelAnimD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
