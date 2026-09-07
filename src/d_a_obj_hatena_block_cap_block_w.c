// @symbol daObjHatenaBlock_c_classInit_CAP_BLOCK_W
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV13QuestionBlock */
/* Reconstructed source-style name: SM64DS proves daObjHatenaBlock_c through RTTI,
 * allocation size, vtable identity, and the CAP_BLOCK_W registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: CapBlockWario_Spawn. */
int *daObjHatenaBlock_c_classInit_CAP_BLOCK_W(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1016);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV13QuestionBlock;
        _ZN9ModelAnimC1Ev((char *)p + 0x320);
        _ZN11ShadowModelC1Ev((char *)p + 0x384);
    }
    return p;
}
