// @symbol _ZN13RollingLogTtmD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int _ZTV8Platform[];
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV13RollingLogTtm; VT1 = _ZTV13daObjMaruta_c */
extern void *data_020a0eac;
extern int _ZTV13RollingLogTtm[];
extern int _ZTV13daObjMaruta_c[];
int *_ZN13RollingLogTtmD0Ev(int *t)
{
    t[0] = (int)_ZTV13RollingLogTtm;
    t[0] = (int)_ZTV13daObjMaruta_c;
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
