// @symbol daBook_c_classInit_SHOOT_BOOK
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAcPos_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV8BookShot */
/* Reconstructed source-style name: SM64DS proves daBook_c through RTTI,
 * allocation size, vtable identity, and the SHOOT_BOOK registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: BookShot_Spawn. */
int *daBook_c_classInit_SHOOT_BOOK(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1108);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV8BookShot;
        _ZN9ModelAnimC1Ev((char *)p + 0x110);
        _ZN5ModelC1Ev((char *)p + 0x174);
        _ZN11ShadowModelC1Ev((char *)p + 0x1c4);
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x21c);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x25c);
    }
    return p;
}
