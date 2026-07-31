// @symbol _ZN6GoombaD1Ev
/* recovered: named members + shared header, vtable identified, globals resolved, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified, globals resolved */
/* resolved: VT0 = _ZTV6Goomba */
extern void _ZN15MaterialChangerD1Ev(void *);
extern void func_ov002_020aedbc(void *);
int *_ZN6GoombaD1Ev(int *t)
{
    t[0] = (int)_ZTV6Goomba;
    _ZN15MaterialChangerD1Ev((char *)t + 0x3fc);
    _ZN11ShadowModelD1Ev((char *)t + 0x3d4);
    _ZN9ModelAnimD1Ev((char *)t + 0x370);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x1b4);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x180);
    func_ov002_020aedbc(t);
    return t;
}
