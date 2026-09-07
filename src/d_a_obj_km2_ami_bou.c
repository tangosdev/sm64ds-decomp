// @symbol daObjKm2_Ami_Bou_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_dCcAc_c.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV8PoleLift */
/* Reconstructed source-style name: SM64DS proves daObjKm2_Ami_Bou_c through
 * RTTI, allocation size, vtable identity, and the KM2_AMI_BOU registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: PoleLift_Spawn. */
int *daObjKm2_Ami_Bou_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(856);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV8PoleLift;
        _ZN7dCcAc_cC1Ev((char *)p + 0x320);
    }
    return p;
}
