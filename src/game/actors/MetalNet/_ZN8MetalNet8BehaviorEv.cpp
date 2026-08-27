//cpp
// @symbol _ZN8MetalNet8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MetalNet.h"
#include "dBgW.h"
extern "C" {
extern int _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void*);
extern int _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
}
extern unsigned char data_0209f2d8;

int MetalNet::Behavior()
{
    int b;
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((void*)this));
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((void*)this));
    b = (int)(data_0209f2d8 == 1);
    if (b != 0) {
        if (((dBgW *)((char*)&mMeshCollider))->IsEnabled() == 0)
            ((dBgW *)((char*)((void*)this)+0x124))->Enable((dActor_c *)(((void*)this)));
    } else {
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((void*)this), 0, 0);
    }
    return 1;
}
