// @symbol _ZN10BulletBillD1Ev
/* recovered: named members + shared header, vtable identified, globals resolved, declarations from a shared header */
#include "decl_Model.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified, globals resolved */
/* resolved: VT0 = _ZTV10BulletBill */
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void func_ov002_020aed18(void *);
int *_ZN10BulletBillD1Ev(int *t)
{
    t[0] = (int)_ZTV10BulletBill;
    _ZN11ShadowModelD1Ev((char *)t + 0x3ac);
    _ZN5ModelD1Ev((char *)t + 0x35c);
    _ZN5ModelD1Ev((char *)t + 0x30c);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x150);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    return t;
}
