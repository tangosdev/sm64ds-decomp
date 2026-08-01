//cpp
// @symbol _ZN8BookShot8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BookShot.h"
extern "C" {
extern void func_0200f760(void* a, void* b);
extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(void* thiz, void* w);
extern void _ZN12CylinderClsn5ClearEv(void* thiz);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void* thiz, const Vector3& v);
extern void _ZN12CylinderClsn6UpdateEv(void* thiz);
}

int BookShot::Behavior()
{
    func_0200f760(((char*)this), ((char*)this) + 0x21c);
    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(((char*)this), ((char*)this) + 0x25c) != 0) {
        if (unk_107 != 0 && unk_104 == 5) {
            unk_428 = mState;
            mState = 5;
            unk_107 = 0;
            unk_0a8 = 0;
            unk_098 = 0x8000;
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
    _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsnWithPos);
    _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(((char*)this) + 0x21c, *(Vector3*)((char*)&unk_438));
    _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsnWithPos);
    return 1;
}
