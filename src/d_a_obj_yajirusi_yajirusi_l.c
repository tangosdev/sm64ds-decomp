// @symbol daObjYajirusi_c_classInit_YAJIRUSI_L
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV14ArrowSignRight */
/* Reconstructed source-style name: SM64DS proves daObjYajirusi_c through RTTI,
 * allocation size, vtable identity, and the YAJIRUSI_L registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: ArrowSignLeft_Spawn. */
int *daObjYajirusi_c_classInit_YAJIRUSI_L(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(896);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV14ArrowSignRight;
        _ZN11ShadowModelC1Ev((char *)p + 0x320);
    }
    return p;
}
