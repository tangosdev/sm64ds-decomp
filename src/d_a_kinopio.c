// @symbol daKinopio_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV4Toad */
/* Reconstructed source-style name: SM64DS proves daKinopio_c through RTTI,
 * allocation size, vtable identity, and the KINOPIO registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Toad_Spawn. */
int *daKinopio_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(528);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV4Toad;
        _ZN7dCcAc_cC1Ev((char *)p + 0xd4);
        _ZN9ModelAnimC1Ev((char *)p + 0x108);
        _ZN11ShadowModelC1Ev((char *)p + 0x16c);
    }
    return p;
}
