// @symbol daDkk_c_classInit
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"
#include "decl_TextureSequence.h"
#include "decl_common.h"
extern int _ZTV7daDkk_c[];
extern int _ZTV11daDsnBase_c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV11daDsnBase_c; VT1 = _ZTV7daDkk_c */
/* Reconstructed source-style name: SM64DS proves daDkk_c through RTTI,
 * allocation size, vtable identity, and the DONKAKU registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Grindel_Spawn. */
int *daDkk_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(928);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV11daDsnBase_c;
        _ZN15TextureSequenceC1Ev((char *)p + 0x324);
        _ZN11ShadowModelC1Ev((char *)p + 0x338);
        p[0] = (int)_ZTV7daDkk_c;
    }
    return p;
}
