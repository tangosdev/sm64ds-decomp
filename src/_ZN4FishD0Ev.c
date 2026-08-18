// @symbol _ZN4FishD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ModelAnim.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV8daFish_c */
extern void *data_020a0eac;
int *_ZN4FishD0Ev(int *t)
{
    t[0] = (int)_ZTV8daFish_c;
    _ZN9ModelAnimD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
