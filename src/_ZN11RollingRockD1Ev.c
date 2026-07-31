// @symbol _ZN11RollingRockD1Ev
/* recovered: named members + shared header, vtable identified, globals resolved, declarations from a shared header */
#include "decl_Model.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified, globals resolved */
/* resolved: VT0 = _ZTV11RollingRock */
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void func_ov002_020aed18(void *);
int *_ZN11RollingRockD1Ev(int *t)
{
    t[0] = (int)_ZTV11RollingRock;
    _ZN12WithMeshClsnD1Ev((char *)t + 0x1f8);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x1b8);
    _ZN11ShadowModelD1Ev((char *)t + 0x160);
    _ZN5ModelD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    return t;
}
