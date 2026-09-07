// @symbol daObjCtMecha11_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV16SpinningPlatform */
/* Reconstructed source-style name: SM64DS proves daObjCtMecha11_c through
 * RTTI, allocation size, vtable identity, and the CT_MECHA11 registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. The project's SpinningPlatform implementation alias remains
 * unchanged. Historical alias: SpinningPlatform_Spawn. */
int *daObjCtMecha11_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(896);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV16SpinningPlatform;
        _ZN11ShadowModelC1Ev((char *)p + 0x328);
    }
    return p;
}
