// @symbol Tornado_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_TextureTransformer.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV7Tornado */
int *Tornado_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(880);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)_ZTV7Tornado;
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0xd4);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x108);
        _ZN9ModelAnimC1Ev((char *)p + 0x2c4);
        _ZN18TextureTransformerC1Ev((char *)p + 0x328);
    }
    return p;
}
