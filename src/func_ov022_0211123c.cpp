//cpp
#include "SharedFilePtr.h"
extern "C" int _ZN16MeshColliderBase9IsEnabledEv(void *);
extern "C" void _ZN16MeshColliderBase7DisableEv(void *);
extern int data_ov022_02113cc8;

extern "C" int func_ov022_0211123c(char *c) {
    if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x124)) {
        _ZN16MeshColliderBase7DisableEv(c + 0x124);
    }
    ((SharedFilePtr *)((void *)((int *)&data_ov022_02113cc8)[0]))->Release();
    ((SharedFilePtr *)((void *)((int *)&data_ov022_02113cc8)[1]))->Release();
    return 1;
}
