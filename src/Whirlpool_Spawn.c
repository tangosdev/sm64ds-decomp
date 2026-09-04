// @symbol daWater_Tatumaki_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_ModelAnim.h"
#include "decl_TextureTransformer.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV9Whirlpool */
/* Reconstructed source-style name: SM64DS proves daWater_Tatumaki_c through RTTI,
 * allocation size, vtable identity, and the WATER_TATUMAKI registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Whirlpool_Spawn. */
int *daWater_Tatumaki_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(444);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV9Whirlpool;
        _ZN9ModelAnimC1Ev((char *)p + 0x114);
        _ZN18TextureTransformerC1Ev((char *)p + 0x178);
    }
    return p;
}
