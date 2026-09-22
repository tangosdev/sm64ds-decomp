// @symbol daObjEmmYuka_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV14SquarePathLift */
extern void _ZN7PathPtrC1Ev(void *);
/* Reconstructed source-style name: SM64DS proves daObjEmmYuka_c through RTTI,
 * allocation size, vtable identity, and the EMM_YUKA registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: SquarePathLift_Spawn. */
int *daObjEmmYuka_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(816);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV14SquarePathLift;
        _ZN7PathPtrC1Ev((char *)p + 0x320);
    }
    return p;
}
