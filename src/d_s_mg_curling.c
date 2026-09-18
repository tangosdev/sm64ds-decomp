// @symbol dScMgCurling_c_classInit
/* recovered: vtable identified, declarations from a shared header */
/* STILL MACHINE-SHAPED (audit 2026-09-18) -- byte-exact; what blocks each part:
 *  1 data_*                      unnamed in config symbols.txt; each needs a
 *                                coined, behaviour-justified name.
 *  2 ctor/dtor/op-new call(s)    C1/C2/D0/D1/D2 is not expressible
 *                                in C++ source; only a real ctor emits it.
 */

#include "decl_ActorBase.h"
#include "decl_common.h"
extern int data_ov006_0213c304[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov006_0213c304 */
/* Reconstructed source-style name: SM64DS proves the dScMgCurling_c RTTI
 * identity, MG_CURLING registry ID, base-profile/factory relationship,
 * allocation size, and vtable identity; later EAD lineage supplies the
 * classInit spelling prior. Exact original SM64DS spelling is not preserved.
 * Historical project alias: MgShuffleShell_Spawn. */
int *dScMgCurling_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(20204);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        p[0] = (int)data_ov006_0213c304;
    }
    return p;
}
