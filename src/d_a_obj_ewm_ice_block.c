// @symbol daObjEwmIceBlock_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV17BigMovingIceBlock */
extern void _ZN7PathPtrC1Ev(void *);
/* Reconstructed source-style name: SM64DS proves daObjEwmIceBlock_c through RTTI,
 * allocation size, vtable identity, and the EWM_ICE_BLOCK registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: BigMovingIceBlock_Spawn. */
int *daObjEwmIceBlock_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(816);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV17BigMovingIceBlock;
        _ZN7PathPtrC1Ev((char *)p + 0x320);
    }
    return p;
}
