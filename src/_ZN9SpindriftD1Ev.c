// @symbol _ZN9SpindriftD1Ev
/* recovered: named members + shared header, vtable identified, globals resolved, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified, globals resolved */
/* resolved: VT0 = _ZTV9Spindrift */
extern void func_ov002_020aed18(void *);
int *_ZN9SpindriftD1Ev(int *t)
{
    t[0] = (int)_ZTV9Spindrift;
    _ZN12WithMeshClsnD1Ev((char *)t + 0x1d0);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x19c);
    _ZN11ShadowModelD1Ev((char *)t + 0x174);
    _ZN9ModelAnimD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    return t;
}
