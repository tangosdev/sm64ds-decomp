//cpp
// @symbol _ZN19FloatOnLavaPlatform8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FloatOnLavaPlatform.h"
extern "C" int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
extern "C" void _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern "C" void _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern "C" void func_020393a4(int* p, int v);

int FloatOnLavaPlatform::Behavior()
{
    func_020393a4((int*)((char*)&(*(u8 *)&mMeshCollider)), 0x150000);
    if (unk_324) {
        int* py = (int*)(((int)((char*)this) + 0x60));
        *py = *py - 0x2000;
        int lim = unk_320 - 0xc8000;
        if (mPosY < lim) mPosY = lim;
        unk_324 = 0;
    } else {
        int* py = (int*)(((int)((char*)this) + 0x60));
        *py = *py + 0x2000;
        int lim = unk_320;
        if (mPosY > lim) mPosY = lim;
    }
    _ZN8Platform21UpdateModelPosAndRotYEv(((char*)this));
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0, 0)) {
        _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
    }
    return 1;
}
