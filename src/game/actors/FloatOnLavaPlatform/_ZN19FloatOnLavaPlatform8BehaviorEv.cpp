//cpp
// @symbol _ZN19FloatOnLavaPlatform8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FloatOnLavaPlatform.h"
extern "C" int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
extern "C" void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
extern "C" void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void*);
extern "C" void func_020393a4(int* p, int v);

int FloatOnLavaPlatform::Behavior()
{
    func_020393a4((int*)((char*)&(*(u8 *)&mMeshCollider)), 0x150000);
    if (mHadClsn) {
        int* py = (int*)(((int)((char*)this) + 0x60));
        *py = *py - 0x2000;
        int lim = mMaxPosY - 0xc8000;
        if (mPosY < lim) mPosY = lim;
        mHadClsn = 0;
    } else {
        int* py = (int*)(((int)((char*)this) + 0x60));
        *py = *py + 0x2000;
        int lim = mMaxPosY;
        if (mPosY > lim) mPosY = lim;
    }
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char*)this));
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0, 0)) {
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
    }
    return 1;
}
