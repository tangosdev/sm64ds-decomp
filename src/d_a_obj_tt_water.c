// @symbol daObjTtWater_c_classInit
/* recovered: globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_TextureTransformer.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT0 = _ZTV9TinyWater */
/* Reconstructed source-style name: SM64DS proves daObjTtWater_c through RTTI,
 * allocation size, vtable identity, and the TT_WATER registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * The project's TinyWater implementation alias remains unchanged.
 * Historical alias: TinyWater_Spawn. */
int *daObjTtWater_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(832);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV9TinyWater;
        _ZN18TextureTransformerC1Ev((char *)p + 0x320);
    }
    return p;
}
