//cpp
// @symbol _ZN8BookShot8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BookShot.h"
extern "C" {
extern void func_0200f760(void* a, void* b);
extern int _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(void* thiz, void* w);
extern void _ZN5dCc_c5ClearEv(void* thiz);
extern void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void* thiz, const Vector3& v);
extern void _ZN5dCc_c6UpdateEv(void* thiz);
}

int BookShot::Behavior()
{
    func_0200f760(((char*)this), ((char*)this) + 0x21c);
    if (_ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(((char*)this), ((char*)this) + 0x25c) != 0) {
        if (mEatenByYoshi != 0 && unk_104 == 5) {
            unk_428 = mState;
            mState = 5;
            mEatenByYoshi = 0;
            mVertSpeed = 0;
            mHorzSpeed = 0x8000;
        }
        func_ov020_0211216c(((char*)this));
        return 1;
    }
    switch (unk_420) {
    case 0:
        func_ov020_02112080(((char*)this));
        break;
    case 1:
        func_ov020_0211174c(((char*)this));
        break;
    }
    func_ov020_0211216c(((char*)this));
    _ZN5dCc_c5ClearEv((char*)&mdCcAcPos_c);
    _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(((char*)this) + 0x21c, *(Vector3*)((char*)&unk_438));
    _ZN5dCc_c6UpdateEv((char*)&mdCcAcPos_c);
    return 1;
}
