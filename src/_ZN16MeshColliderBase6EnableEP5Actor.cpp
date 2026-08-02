//cpp
// @symbol _ZN16MeshColliderBase6EnableEP5Actor
#include "MeshColliderBase.h"
extern "C" {
void func_020395fc(MeshColliderBase *self, Actor *actor);
void func_020393fc(MeshColliderBase *self, int slot);
extern MeshColliderBase *data_020a0c80[];
}

int MeshColliderBase::Enable(Actor *actor)
{
    s32 i = 0;
    for (;;) {
        if (data_020a0c80[i] == 0) {
            func_020395fc(this, actor);
            func_020393fc(this, i);
            data_020a0c80[i] = this;
            return 1;
        }
        i++;
        if (i >= 0x18)
            break;
    }
    return 0;
}
