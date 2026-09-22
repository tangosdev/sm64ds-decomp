// @symbol daObjRotateUpdownLift_c_classInit_HS_UPDOWN_LIFT
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV25RotatingUpDownPlatformUtm */
/* Reconstructed source-style name: SM64DS proves daObjRotateUpdownLift_c through RTTI,
 * allocation size, vtable identity, and the HS_UPDOWN_LIFT registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: RotatingUpDownPlatformUtm_Spawn. */
int *daObjRotateUpdownLift_c_classInit_HS_UPDOWN_LIFT(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(936);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV25RotatingUpDownPlatformUtm;
        _ZN11ShadowModelC1Ev((char *)p + 0x320);
    }
    return p;
}
