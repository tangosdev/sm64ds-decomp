//cpp
// @symbol _ZN9HugeWater8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "HugeWater.h"
extern "C" {
extern void _ZN9Animation7AdvanceEv(void *);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *);
}

int HugeWater::Behavior()
{
    (*(s32 *)((char *)&mTextureTransformer + 0xc)) = 4096;
    _ZN9Animation7AdvanceEv((char *)&(*(u8 *)&mTextureTransformer));
    _ZN8Platform21UpdateModelPosAndRotYEv(((char *)this));
    _ZN8Platform19UpdateClsnPosAndRotEv(((char *)this));
    return 1;
}
