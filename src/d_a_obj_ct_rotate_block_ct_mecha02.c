// @symbol daObjCtRotateBlock_c_classInit_CT_MECHA02
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV15TtcRotatingCube */
/* Reconstructed source-style name: SM64DS proves daObjCtRotateBlock_c through RTTI,
 * allocation size, vtable identity, and the CT_MECHA02 registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: TtcRotatingPrism_Spawn. */
int *daObjCtRotateBlock_c_classInit_CT_MECHA02(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(984);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV15TtcRotatingCube;
        _ZN5ModelC1Ev((char *)p + 0x320);
        _ZN11ShadowModelC1Ev((char *)p + 0x380);
    }
    return p;
}
