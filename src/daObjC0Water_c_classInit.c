// @symbol daObjC0Water_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_TextureTransformer.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV14daObjC0Water_c */
/* Reconstructed source-style name: SM64DS proves daObjC0Water_c through RTTI,
 * allocation size, vtable identity, and the C0_WATER registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: daObjC0Water_c_Spawn. */
int *daObjC0Water_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(832);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV14daObjC0Water_c;
        _ZN18TextureTransformerC1Ev((char *)p + 0x320);
    }
    return p;
}
