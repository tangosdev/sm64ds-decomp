// @symbol daObjHmBskt_c_classInit
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
extern int _ZTV13daObjHmBskt_c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV13daObjHmBskt_c */
/* Reconstructed source-style name: SM64DS proves daObjHmBskt_c through RTTI,
 * allocation size, vtable identity, and the HM_BASKET registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * The project's UkikiCage implementation aliases remain unchanged.
 * Historical alias: UkikiCage_Spawn. */
int *daObjHmBskt_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1248);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV13daObjHmBskt_c;
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x320);
    }
    return p;
}
