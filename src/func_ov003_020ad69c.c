// @symbol func_ov003_020ad69c
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
extern void _ZN9ActorBaseD2Ev(void *self);
extern void*_ZTV5Scene[];
extern int data_0208e4b8[];
extern void *data_020a0eac;

int *func_ov003_020ad69c(int *t)
{
    t[0] = (int)data_ov003_020b1650;
    t[0] = (int)_ZTV5Scene;
    t[0] = (int)data_0208e4b8;
    _ZN9ActorBaseD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
