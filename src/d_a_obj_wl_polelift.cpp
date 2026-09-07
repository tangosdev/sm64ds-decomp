//cpp
// @symbol daObjWlPolelift_c_classInit
#include "daObjWlPolelift_c.h"

/* CodeWarrior rejects the actor hierarchy's class-specific operator-new
 * declaration. Keep that factory ABI explicit while naming the real class,
 * vtable, and every subobject in the construction sequence. */
extern "C" {
void *_ZN7fBase_cnwEj(u32 size);
void _ZN8dActor_cC2Ev(void *self);
void _ZN5ModelC1Ev(void *self);
void _ZN10dCcAcPos_cC1Ev(void *self);
void _ZN7PathPtrC1Ev(void *self);
void _ZN11ShadowModelC1Ev(void *self);
extern void *_ZTV17daObjWlPolelift_c;
}

/* Reconstructed source-style name: SM64DS proves daObjWlPolelift_c through
 * RTTI, allocation size, vtable identity, and the WL_POLELIFT registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical placeholder: func_ov026_02111678. */
extern "C" daObjWlPolelift_c *daObjWlPolelift_c_classInit()
{
    daObjWlPolelift_c *actor =
        (daObjWlPolelift_c *)_ZN7fBase_cnwEj(sizeof(daObjWlPolelift_c));
    if (actor) {
        _ZN8dActor_cC2Ev(actor);
        *(void **)actor = &_ZTV17daObjWlPolelift_c;
        _ZN5ModelC1Ev(&actor->model);
        _ZN10dCcAcPos_cC1Ev(&actor->clsn);
        _ZN7PathPtrC1Ev(&actor->path);
        _ZN11ShadowModelC1Ev(&actor->shadow);
    }
    return actor;
}
