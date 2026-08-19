//cpp
// @symbol _ZN16MeshColliderBase7DisableEv
#include "MeshColliderBase.h"
extern "C" {
int func_02039404(MeshColliderBase *self);
void func_020393f0(MeshColliderBase *self);
extern MeshColliderBase *data_020a0c80[];
}

int MeshColliderBase::Disable()
{
    int i = func_02039404(this);
    if (i != 0x18) {
        func_020393f0(this);
        data_020a0c80[i] = 0;
    }
    return 1;
}
