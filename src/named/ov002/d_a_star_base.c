// @symbol daStarBase_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_dCcAcPos_c.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV12daStarBase_c */
/* Reconstructed source-style name: SM64DS proves daStarBase_c through RTTI,
 * allocation size, vtable identity, and the STARBASE registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: StarMarker_Spawn. */
int *daStarBase_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(476);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV12daStarBase_c;
        _ZN10dCcAcPos_cC1Ev((char *)p + 0xd4);
        _ZN5ModelC1Ev((char *)p + 0x114);
        _ZN17dExtShadowModel_cC1Ev((char *)p + 0x164);
    }
    return p;
}
