// @symbol _ZN10BrickBlockD0Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "BrickBlock.h"
extern void* data_020a0eac;
extern int _ZTV10BrickBlock[];
int *_ZN10BrickBlockD0Ev(int *t)
{
    t[0] = (int)_ZTV10BrickBlock;
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
