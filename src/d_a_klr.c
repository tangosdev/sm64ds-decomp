// @symbol daKlr_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_Model.h"
#include "decl_dCcAcPos_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV10BulletBill */
/* Reconstructed source-style name: SM64DS proves daKlr_c through RTTI,
 * allocation size, vtable identity, and the KILLER registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: BulletBill_Spawn. */
int *daKlr_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(992);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV10BulletBill;
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x110);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x150);
        _ZN5ModelC1Ev((char *)p + 0x30c);
        _ZN5ModelC1Ev((char *)p + 0x35c);
        _ZN11ShadowModelC1Ev((char *)p + 0x3ac);
    }
    return p;
}
