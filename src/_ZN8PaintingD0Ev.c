// @symbol _ZN8PaintingD0Ev
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_common.h"
extern void* data_020a0eac;
extern int _ZTV8Painting[];
/* recovered: renamed to Class_Method */
int *_ZN8PaintingD0Ev(int *t)
{
    t[0] = (int)_ZTV8Painting;
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
