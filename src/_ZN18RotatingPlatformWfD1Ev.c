// @symbol _ZN18RotatingPlatformWfD1Ev
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int _ZTV18RotatingPlatformWf[];
extern int data_ov002_021091d4[];
extern int _ZTV8Platform[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV18RotatingPlatformWf; VT1 = data_ov002_021091d4 */
int *_ZN18RotatingPlatformWfD1Ev(int *t)
{
    t[0] = (int)_ZTV18RotatingPlatformWf;
    t[0] = (int)data_ov002_021091d4;
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
