// @symbol Pokey_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV5Pokey */
int *Pokey_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(944);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)_ZTV5Pokey;
        _ZN5ModelC1Ev((char *)p + 0xd4);
        _ZN11ShadowModelC1Ev((char *)p + 0x124);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x14c);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x180);
    }
    return p;
}
