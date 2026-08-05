// @symbol _ZN8MugenBgmD0Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "MugenBgm.h"
extern void* data_020a0eac;
extern int _ZTV8MugenBgm[];
int *_ZN8MugenBgmD0Ev(int *t)
{
    t[0] = (int)_ZTV8MugenBgm;
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
