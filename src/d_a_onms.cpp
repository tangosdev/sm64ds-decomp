//cpp
// @symbol daOnms_c_classInit
#include "ToxBox.h"

/* CodeWarrior rejects the cartridge's class-specific operator-new spelling
 * and placement new. Keep that factory ABI explicit while naming every base
 * and member subobject the original construction sequence initializes. */
extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN10dBgActor_cC2Ev(void *self);
void _ZN10dBgCh_ActrC1Ev(void *self);
void _ZN10dCcAcPos_cC1Ev(void *self);
void _ZN7PathPtrC1Ev(void *self);
extern void *_ZTV6ToxBox;
}

/* Reconstructed source-style name: SM64DS proves daOnms_c through RTTI,
 * allocation size, vtable identity, and the ONIMASU registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: ToxBox_Spawn. */
extern "C" ToxBox *daOnms_c_classInit()
{
    ToxBox *actor = (ToxBox *)_ZN7fBase_cnwEj(sizeof(ToxBox));
    if (actor) {
        _ZN10dBgActor_cC2Ev(actor);
        *(void **)actor = &_ZTV6ToxBox;
        _ZN10dBgCh_ActrC1Ev(&actor->mWithMeshClsn);
        _ZN10dCcAcPos_cC1Ev(&actor->mdCcAcPos_c);
        _ZN7PathPtrC1Ev(&actor->mPathPtr);
    }
    return actor;
}
