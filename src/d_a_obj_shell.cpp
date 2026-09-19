//cpp
// @symbol daObjShell_c_classInit
#include "daObjShell_c.h"

/* This compiler rejects both the cartridge's class-specific operator-new
 * spelling and placement new. Keep that allocation/constructor ABI boundary
 * explicit, while the real class layout names every constructed subobject. */
extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN8dActor_cC2Ev(void *self);
void _ZN9ModelAnimC1Ev(void *self);
void _ZN7dCcAc_cC1Ev(void *self);
extern void *_ZTV12daObjShell_c;
}

/* Reconstructed source-style name: SM64DS proves daObjShell_c through RTTI,
 * allocation size, vtable identity, and the OBJ_SHELL registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Clam_Spawn. */
extern "C" daObjShell_c *daObjShell_c_classInit()
{
    daObjShell_c *actor = (daObjShell_c *)_ZN7fBase_cnwEj(sizeof(daObjShell_c));
    if (actor) {
        _ZN8dActor_cC2Ev(actor);
        *(void **)actor = &_ZTV12daObjShell_c;
        _ZN9ModelAnimC1Ev(&actor->mModelAnim);
        _ZN7dCcAc_cC1Ev(&actor->mdCcAc_c);
    }
    return actor;
}
