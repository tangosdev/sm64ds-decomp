// @symbol daObjCtKaitendai_c_classInit_CT_MECHA06
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV13TTC_MovingBar */
/* Reconstructed source-style name: SM64DS proves daObjCtKaitendai_c through RTTI,
 * allocation size, vtable identity, and the CT_MECHA06 registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: TtcRotatingGear_Spawn. */
int *daObjCtKaitendai_c_classInit_CT_MECHA06(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(892);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV13TTC_MovingBar;
        _ZN11ShadowModelC1Ev((char *)p + 0x324);
    }
    return p;
}
