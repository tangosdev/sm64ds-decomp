// @symbol _ZN10KingBobOmbD1Ev
/* recovered: named members + shared header, vtable identified, globals resolved, declarations from a shared header */
#include "decl_BlendModelAnim.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified, globals resolved */
/* resolved: VT0 = _ZTV10KingBobOmb */
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void _ZN5EnemyD2Ev(void *);
int *_ZN10KingBobOmbD1Ev(int *t)
{
    t[0] = (int)_ZTV10KingBobOmb;
    _ZN11ShadowModelD1Ev((char *)t + 0x3f8);
    _ZN11CommonModelD1Ev((char *)t + 0x3bc);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x37c);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x33c);
    _ZN14BlendModelAnimD1Ev((char *)t + 0x2cc);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x110);
    _ZN5EnemyD2Ev(t);
    return t;
}
