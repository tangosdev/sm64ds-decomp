// @symbol daSBird_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV4Bird */
/* Reconstructed source-style name: SM64DS proves daSBird_c through RTTI,
 * allocation size, vtable identity, and the SBIRD registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Bird_Spawn. */
int *daSBird_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(388);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV4Bird;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN11ShadowModelC1Ev((char *)p + 0x138);
    }
    return p;
}
