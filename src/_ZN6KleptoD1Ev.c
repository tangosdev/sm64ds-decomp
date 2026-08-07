// @symbol _ZN6KleptoD1Ev
/* recovered: named members + shared header, vtable identified, globals resolved, declarations from a shared header */
#include "decl_BlendModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified, globals resolved */
/* resolved: VT0 = _ZTV6Klepto */
extern void _ZN5EnemyD2Ev(void *);
int *_ZN6KleptoD1Ev(int *t)
{
    t[0] = (int)_ZTV6Klepto;
    _ZN11ShadowModelD1Ev((char *)t + 0x3a4);
    _ZN14BlendModelAnimD1Ev((char *)t + 0x334);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x178);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x144);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x110);
    _ZN5EnemyD2Ev(t);
    return t;
}
