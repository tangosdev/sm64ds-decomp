// @symbol daWater_Ring_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_Model.h"
#include "decl_dCcAcPos_c.h"
#include "decl_TextureTransformer.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV9WaterRing */
/* Reconstructed source-style name: SM64DS proves daWater_Ring_c through RTTI,
 * allocation size, vtable identity, and the WATER_RING registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: WaterRing_Spawn. */
int *daWater_Ring_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(912);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV9WaterRing;
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x110);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x150);
        _ZN5ModelC1Ev((char *)p + 0x30c);
        _ZN18TextureTransformerC1Ev((char *)p + 0x35c);
    }
    return p;
}
