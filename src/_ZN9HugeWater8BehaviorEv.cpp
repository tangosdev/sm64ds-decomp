//cpp
// @symbol _ZN9HugeWater8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "HugeWater.h"
extern "C" {
extern void _ZN9Animation7AdvanceEv(void *);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *);
}

int HugeWater::Behavior()
{
    (*(s32 *)((char *)&mTextureTransformer + 0xc)) = 4096;
    _ZN9Animation7AdvanceEv((char *)&(*(u8 *)&mTextureTransformer));
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char *)this));
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char *)this));
    return 1;
}
