// @symbol daLinelift2_c_classInit_KM3_LIFT
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV22RotatingUpDownPlatform */
extern void _ZN7PathPtrC1Ev(void *);
/* Reconstructed source-style name: SM64DS proves daLinelift2_c through RTTI,
 * allocation size, vtable identity, and the KM3_LIFT registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: ArrowPathLift_Spawn. */
int *daLinelift2_c_classInit_KM3_LIFT(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(856);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV22RotatingUpDownPlatform;
        _ZN7PathPtrC1Ev((char *)p + 0x344);
    }
    return p;
}
