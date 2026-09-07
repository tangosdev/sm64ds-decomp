// @symbol daEykn_c_classInit_EYEKUN_BOSS
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAcPos_c.h"
#include "decl_ShadowModel.h"
#include "decl_TextureSequence.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV3MrI */
/* Reconstructed source-style name: SM64DS proves daEykn_c through RTTI,
 * allocation size, vtable identity, and the EYEKUN_BOSS registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: BigMrI_Spawn. */
int *daEykn_c_classInit_EYEKUN_BOSS(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(536);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV3MrI;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN15TextureSequenceC1Ev((char *)p + 0x138);
        _ZN11ShadowModelC1Ev((char *)p + 0x14c);
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x174);
    }
    return p;
}
