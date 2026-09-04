//cpp
// @symbol Fish_Spawn
#include "Fish.h"
#include "decl_common.h"

extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN8dActor_cC2Ev(void *self);
void _ZN9ModelAnimC1Ev(void *self);
void func_0203b9b4(int *value, int initial);
}

extern "C" Fish *Fish_Spawn()
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
