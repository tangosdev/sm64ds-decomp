//cpp
// @symbol _ZN8MetalNet8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MetalNet.h"
#include "MeshColliderBase.h"
extern "C" {
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
}
extern unsigned char data_0209f2d8;

int MetalNet::Behavior()
{
    int b;
    _ZN8Platform21UpdateModelPosAndRotYEv(((void*)this));
    _ZN8Platform19UpdateClsnPosAndRotEv(((void*)this));
    b = (int)(data_0209f2d8 == 1);
    if (b != 0) {
        if (((MeshColliderBase *)((char*)&mMovingMeshCollider))->IsEnabled() == 0)
            ((MeshColliderBase *)((char*)((void*)this)+0x124))->Enable((Actor *)(((void*)this)));
    } else {
        _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((void*)this), 0, 0);
    }
    return 1;
}
