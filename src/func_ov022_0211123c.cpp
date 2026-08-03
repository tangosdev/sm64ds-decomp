//cpp
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"
extern int data_ov022_02113cc8;

extern "C" int func_ov022_0211123c(char *c) {
    if (((MeshColliderBase *)(c + 0x124))->IsEnabled()) {
        ((MeshColliderBase *)(c + 0x124))->Disable();
    }
    ((SharedFilePtr *)((void *)((int *)&data_ov022_02113cc8)[0]))->Release();
    ((SharedFilePtr *)((void *)((int *)&data_ov022_02113cc8)[1]))->Release();
    return 1;
}
