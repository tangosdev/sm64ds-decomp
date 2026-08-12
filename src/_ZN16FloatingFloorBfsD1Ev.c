// @symbol _ZN16FloatingFloorBfsD1Ev
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int _ZTV16FloatingFloorBfs[];
extern int data_ov002_0210912c[];
extern int _ZTV8Platform[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV16FloatingFloorBfs; VT1 = data_ov002_0210912c */
int *_ZN16FloatingFloorBfsD1Ev(int *t)
{
    t[0] = (int)_ZTV16FloatingFloorBfs;
    t[0] = (int)data_ov002_0210912c;
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
