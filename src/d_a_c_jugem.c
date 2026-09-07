// @symbol daC_Jugem_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_ModelAnim.h"
#include "decl_ShadowModel.h"
#include "decl_TextureSequence.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV9LakituBro */
/* Reconstructed source-style name: SM64DS proves daC_Jugem_c through RTTI,
 * allocation size, vtable identity, and the C_JUGEM registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: LakituBro_Spawn. */
int *daC_Jugem_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(744);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV9LakituBro;
        _ZN9ModelAnimC1Ev((char *)p + 0x110);
        _ZN9ModelAnimC1Ev((char *)p + 0x174);
        _ZN15TextureSequenceC1Ev((char *)p + 0x1d8);
        _ZN11ShadowModelC1Ev((char *)p + 0x1f0);
        _ZN11ShadowModelC1Ev((char *)p + 0x218);
    }
    return p;
}
