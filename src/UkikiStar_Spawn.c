// @symbol UkikiStar_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV13RollingLogTtm */
extern void _ZN7PathPtrC1Ev(void *);
int *UkikiStar_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(972);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)_ZTV13RollingLogTtm;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN11ShadowModelC1Ev((char *)p + 0x138);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x160);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x194);
        _ZN7PathPtrC1Ev((char *)p + 0x398);
    }
    return p;
}
