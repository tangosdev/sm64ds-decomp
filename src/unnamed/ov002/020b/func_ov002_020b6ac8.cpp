//cpp
#include "SharedFilePtr.h"
#include "dBgW.h"
extern "C" {
int func_ov002_020b6ac8(void* c, void* r4) {
    if (((dBgW *)((char*)c+0x124))->IsEnabled())
        ((dBgW *)((char*)c+0x124))->Disable();
    ((SharedFilePtr *)(*(void**)r4))->Release();
    ((SharedFilePtr *)(*(void**)((char*)r4+4)))->Release();
    return 1;
}
}
