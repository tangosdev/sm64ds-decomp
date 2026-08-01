// @symbol func_ov026_02111678
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_MovingCylinderClsnWithPos.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV17daObjWlPolelift_c */
extern void _ZN7PathPtrC1Ev(void *);
int *func_ov026_02111678(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(484);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)_ZTV17daObjWlPolelift_c;
        _ZN5ModelC1Ev((char *)p + 0xd4);
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x124);
        _ZN7PathPtrC1Ev((char *)p + 0x164);
        _ZN11ShadowModelC1Ev((char *)p + 0x188);
    }
    return p;
}
