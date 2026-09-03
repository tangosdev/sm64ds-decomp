// @symbol daObjTdWater_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_TextureTransformer.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV14daObjTdWater_c */
/* Reconstructed source-style name: SM64DS proves daObjTdWater_c through RTTI,
 * allocation size, vtable identity, and the TD_WATER registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * The project's daObjTdWater_c implementation alias remains unchanged.
 * Historical alias: HugeWater_Spawn. */
int *daObjTdWater_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(820);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV14daObjTdWater_c;
        _ZN18TextureTransformerC1Ev((char *)p + 0x320);
    }
    return p;
}
