// @symbol _ZN13RollingLogLllD1Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int _ZTV8Platform[];
extern int _ZTV13RollingLogLll[];
extern int _ZTV13daObjMaruta_c[];
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV13RollingLogLll; VT1 = _ZTV13daObjMaruta_c */
int *_ZN13RollingLogLllD1Ev(int *t)
{
    t[0] = (int)_ZTV13RollingLogLll;
    t[0] = (int)_ZTV13daObjMaruta_c;
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
