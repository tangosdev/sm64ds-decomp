//cpp
#include "SharedFilePtr.h"
extern "C" {
extern int _ZN16MeshColliderBase9IsEnabledEv(void*);
extern void _ZN16MeshColliderBase7DisableEv(void*);
int func_ov002_020b68b0(void* c, void* r4) {
    if (_ZN16MeshColliderBase9IsEnabledEv((char*)c+0x124))
        _ZN16MeshColliderBase7DisableEv((char*)c+0x124);
    ((SharedFilePtr *)(*(void**)r4))->Release();
    ((SharedFilePtr *)(*(void**)((char*)r4+4)))->Release();
    return 1;
}
}
