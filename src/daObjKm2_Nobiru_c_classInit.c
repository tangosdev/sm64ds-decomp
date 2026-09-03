// @symbol daObjKm2_Nobiru_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_dBgW_KcMbgSclY.h"
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV17daObjKm2_Nobiru_c */
/* Reconstructed source-style name: SM64DS proves daObjKm2_Nobiru_c through
 * RTTI, allocation size, vtable identity, and the KM2_NOBIRU registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: ExtendingPlatform_Spawn. */
int *daObjKm2_Nobiru_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(808);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV17daObjKm2_Nobiru_c;
        _ZN5ModelC1Ev((char *)p + 0xd8);
        _ZN14dBgW_KcMbgSclYC1Ev((char *)p + 0x158);
    }
    return p;
}
