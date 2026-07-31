// @symbol func_ov026_021111a0
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV17daObjWlPolelift_c */
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
int *func_ov026_021111a0(int *t)
{
    t[0] = (int)_ZTV17daObjWlPolelift_c;
    _ZN11ShadowModelD1Ev((char *)t + 0x188);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
