// @symbol _ZN10BowserFireD1Ev
/* recovered: named members + shared header, vtable identified, globals resolved, declarations from a shared header */
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified, globals resolved */
/* resolved: VT0 = _ZTV10BowserFire */
extern void func_ov002_020aed18(void *);
int *_ZN10BowserFireD1Ev(int *t)
{
    t[0] = (int)_ZTV10BowserFire;
    _ZN11ShadowModelD1Ev((char *)t + 0x304);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x2d0);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    return t;
}
