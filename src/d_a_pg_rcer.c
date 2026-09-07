// @symbol daPgRcer_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_TextureSequence.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV13RacingPenguin */
extern void _ZN7PathPtrC1Ev(void *);
/* Reconstructed source-style name: SM64DS proves daPgRcer_c through RTTI,
 * allocation size, vtable identity, and the PENGUIN_RACER registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: RacingPenguin_Spawn. */
int *daPgRcer_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(920);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV13RacingPenguin;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN15TextureSequenceC1Ev((char *)p + 0x138);
        _ZN11ShadowModelC1Ev((char *)p + 0x14c);
        _ZN7dCcAc_cC1Ev((char *)p + 0x174);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x1a8);
        _ZN7PathPtrC1Ev((char *)p + 0x364);
    }
    return p;
}
