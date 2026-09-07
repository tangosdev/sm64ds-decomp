// @symbol daTor_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_TextureTransformer.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV7Tornado */
/* Reconstructed source-style name: SM64DS proves daTor_c through RTTI,
 * allocation size, vtable identity, and the TORNADO registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Tornado_Spawn. */
int *daTor_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(880);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV7Tornado;
        _ZN7dCcAc_cC1Ev((char *)p + 0xd4);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x108);
        _ZN9ModelAnimC1Ev((char *)p + 0x2c4);
        _ZN18TextureTransformerC1Ev((char *)p + 0x328);
    }
    return p;
}
