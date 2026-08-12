// @symbol _ZN19RotatingPlatformWdwD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int _ZTV19RotatingPlatformWdw[];
extern int data_ov002_021091d4[];
extern int _ZTV8Platform[];
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV19RotatingPlatformWdw; VT1 = data_ov002_021091d4 */
extern void *data_020a0eac;
int *_ZN19RotatingPlatformWdwD0Ev(int *t)
{
    t[0] = (int)_ZTV19RotatingPlatformWdw;
    t[0] = (int)data_ov002_021091d4;
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
