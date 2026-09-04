// @symbol BigBooIcon_Spawn
/* CodeWarrior 2004/b56 rejects every honest source spelling for construction
 * into allocator-returned storage (notes/ctor-migration.md, factory wall), so
 * this folded factory remains the exact C transcription. Its 0xd8 allocation,
 * dActor_c base step, and daTrsIcon_c vptr still prove the shared class layout. */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_common.h"

int *BigBooIcon_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(216);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV10BigBooIcon;
    }
    return p;
}
