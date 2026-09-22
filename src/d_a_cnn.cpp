//cpp
// @symbol daCnn_c_classInit
#include "Cannon.h"

/* This compiler rejects both the cartridge's class-specific operator-new
 * spelling and placement new. Keep that allocation/constructor ABI boundary
 * explicit, while the real class layout names every constructed subobject. */
extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN8dActor_cC2Ev(void *self);
void _ZN5ModelC1Ev(void *self);
void _ZN7dCcAc_cC1Ev(void *self);
extern void *_ZTV6Cannon;
}

/* Reconstructed source-style name: SM64DS proves daCnn_c through RTTI,
 * allocation size, vtable identity, and the CANNON registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Cannon_Spawn. */
extern "C" Cannon *daCnn_c_classInit()
{
    Cannon *actor = (Cannon *)_ZN7fBase_cnwEj(sizeof(Cannon));
    if (actor) {
        _ZN8dActor_cC2Ev(actor);
        *(void **)actor = &_ZTV6Cannon;
        _ZN5ModelC1Ev(&actor->mModel);
        _ZN7dCcAc_cC1Ev(&actor->mdCcAc_c);
    }
    return actor;
}
