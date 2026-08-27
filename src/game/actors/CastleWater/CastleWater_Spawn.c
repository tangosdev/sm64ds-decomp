#include "daObjMcWater_c.h"
// @symbol CastleWater_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_TextureTransformer.h"
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV14daObjMcWater_c */
int *CastleWater_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(sizeof(struct daObjMcWater_c));
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV14daObjMcWater_c;
        _ZN18TextureTransformerC1Ev((char *)p + 0x320);
    }
    return p;
}
