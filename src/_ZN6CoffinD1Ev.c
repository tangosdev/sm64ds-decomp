// @symbol _ZN6CoffinD1Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int _ZTV6Coffin[];
extern int _ZTV10dBgActor_c[];
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV6Coffin */
int *_ZN6CoffinD1Ev(int *t)
{
    t[0] = (int)_ZTV6Coffin;
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    return t;
}
