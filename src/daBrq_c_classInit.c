// @symbol daBrq_c_classInit
/* The pinned compiler has no source spelling for placement construction with
 * retail's fBase_c allocator. Keep this measured factory seam in C while the
 * real daBrq_c TU owns its methods, destructor, RTTI, and vtable.
 *
 * Reconstructed source-style name: SM64DS proves the daBrq_c RTTI identity,
 * BIRIKYU registry ID, descriptor relationship, and object shape; later EAD
 * lineage supplies the classInit spelling prior. Exact original SM64DS
 * spelling is not preserved. Historical project alias: Amp_Spawn. */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAcPos_c.h"
#include "decl_ShadowModel.h"
#include "decl_TextureSequence.h"
#include "decl_TextureTransformer.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"

/* resolved: retail class identity is _ZTV7daBrq_c */
int *daBrq_c_classInit(void)
{
    int *actor = (int *)_ZN7fBase_cnwEj(0x434);
    if (actor) {
        _ZN8dActor_cC2Ev(actor);
        actor[0] = (int)_ZTV7daBrq_c;
        _ZN9ModelAnimC1Ev((char *)actor + 0x0d4);
        _ZN5ModelC1Ev((char *)actor + 0x138);
        _ZN15TextureSequenceC1Ev((char *)actor + 0x188);
        _ZN18TextureTransformerC1Ev((char *)actor + 0x19c);
        _ZN11ShadowModelC1Ev((char *)actor + 0x1b0);
        _ZN10dCcAcPos_cC1Ev((char *)actor + 0x1d8);
        _ZN10dBgCh_ActrC1Ev((char *)actor + 0x218);
    }
    return actor;
}
