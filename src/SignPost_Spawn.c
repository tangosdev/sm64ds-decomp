// @symbol SignPost_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV8SignPost */
int *SignPost_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1444);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV8SignPost;
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x320);
        _ZN11ShadowModelC1Ev((char *)p + 0x358);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x3c8);
    }
    return p;
}
