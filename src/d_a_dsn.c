// @symbol daDsn_c_classInit
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"
#include "decl_TextureSequence.h"
#include "decl_common.h"
extern int _ZTV11daDsnBase_c[];
extern int _ZTV6Thwomp[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV11daDsnBase_c */
/* Reconstructed source-style name: SM64DS proves daDsn_c through RTTI,
 * allocation size, vtable identity, and the DOSUN registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Thwomp_Spawn. */
int *daDsn_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(932);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV11daDsnBase_c;
        _ZN15TextureSequenceC1Ev((char *)p + 0x324);
        _ZN11ShadowModelC1Ev((char *)p + 0x338);
        p[0] = (int)_ZTV6Thwomp;
    }
    return p;
}
