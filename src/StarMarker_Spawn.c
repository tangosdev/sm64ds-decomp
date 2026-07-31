// @symbol StarMarker_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_MovingCylinderClsnWithPos.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV10StarMarker */
int *StarMarker_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(476);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)_ZTV10StarMarker;
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0xd4);
        _ZN5ModelC1Ev((char *)p + 0x114);
        _ZN11ShadowModelC1Ev((char *)p + 0x164);
    }
    return p;
}
