// @symbol daObjFl_Amilift_c_classInit
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV12MetalNetLift[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV12MetalNetLift */
extern void _ZN7PathPtrC1Ev(void *);
/* Reconstructed source-style name: SM64DS proves daObjFl_Amilift_c through
 * RTTI, allocation size, vtable identity, and the FL_AMILIFT registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. The project's MetalNetLift implementation aliases remain
 * unchanged. Historical alias: MetalNetLift_Spawn. */
int *daObjFl_Amilift_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(872);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV12MetalNetLift;
        _ZN7PathPtrC1Ev((char *)p + 0x360);
    }
    return p;
}
