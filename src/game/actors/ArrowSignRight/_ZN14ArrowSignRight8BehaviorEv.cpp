//cpp
// @symbol _ZN14ArrowSignRight8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "ArrowSignRight.h"
extern "C" {
int _ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(void* c, short a, short b, short d, int e);
void func_02039394(int* p, int v);
void func_020393a4(int* p, int v);
void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void* c, void* sm, void* mtx, int s, int x, int y, unsigned int j);
int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void* c, int a, int b);
}

int ArrowSignRight::Behavior()
{
    if (_ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(((char*)this), -0x2000, 0, 0, 0x96000))
        return 1;
    func_02039394((int*)((char*)&(*(u8 *)&mMeshCollider)), 0xc0000);
    func_020393a4((int*)((char*)&(*(u8 *)&mMeshCollider)), 0xe0000);
    _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        ((char*)this), (void*)((char*)&mShadowModel), (void*)((char*)&mShadowMat), 0x10e000, 0x64000, 0x46000, 0xf);
    _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(((char*)this), 0x600000, 0);
    return 1;
}
