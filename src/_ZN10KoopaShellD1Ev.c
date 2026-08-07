// @symbol _ZN10KoopaShellD1Ev
/* recovered: named members + shared header, vtable identified, globals resolved, declarations from a shared header */
#include "decl_Model.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified, globals resolved */
/* resolved: VT0 = _ZTV10KoopaShell */
extern void _ZN5EnemyD2Ev(void *);
int *_ZN10KoopaShellD1Ev(int *t)
{
    t[0] = (int)_ZTV10KoopaShell;
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x378);
    _ZN11ShadowModelD1Ev((char *)t + 0x350);
    _ZN5ModelD1Ev((char *)t + 0x300);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x144);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x110);
    _ZN5EnemyD2Ev(t);
    return t;
}
