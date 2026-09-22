//cpp
// @symbol daFish_c_classInit
#include "Fish.h"
#include "decl_common.h"

extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN8dActor_cC2Ev(void *self);
void _ZN9ModelAnimC1Ev(void *self);
void func_0203b9b4(int *value, int initial);
}

/* Reconstructed source-style name: SM64DS proves daFish_c through RTTI,
 * allocation size, vtable identity, and the FISH registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Fish_Spawn. */
extern "C" Fish *daFish_c_classInit()
{
    Fish *actor = (Fish *)_ZN7fBase_cnwEj(sizeof(Fish));
    if (actor) {
        _ZN8dActor_cC2Ev(actor);
        *(void **)actor = _ZTV4Fish;
        _ZN9ModelAnimC1Ev(&actor->mModelAnim);
        func_0203b9b4(&actor->unk_138, 1);
    }
    return actor;
}
