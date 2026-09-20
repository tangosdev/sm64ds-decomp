//cpp
// @symbol _ZN8BookShot8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BookShot.h"
extern "C" {
extern void func_0200f760(void* self, char* actor);
}

int BookShot::Behavior()
{
    func_0200f760(this, (char*)&mdCcAcPos_c);
    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
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
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.SetPosRelativeToActor(*(const Vector3*)&unk_438);
    mdCcAcPos_c.Update();
    return 1;
}
