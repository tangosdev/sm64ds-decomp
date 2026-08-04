// @symbol _ZN5EnemyD0Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Enemy.h"
extern void* data_020a0eac;
extern int data_ov002_021081e4[];
int *_ZN5EnemyD0Ev(int *t)
{
    t[0] = (int)data_ov002_021081e4;
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
