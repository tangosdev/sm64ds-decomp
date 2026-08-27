//cpp
// @symbol _ZN11BabyPenguin8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "BabyPenguin.h"
extern "C" {
int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void* self, int d);
void func_ov072_02120d04(void* c);
void func_ov072_02121cdc(void* c);
void func_ov072_021210c4(void* c);
}

int BabyPenguin::Behavior()
{
    if (_ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(this, 0x7d0000) && mWithMeshClsn.IsOnGround()) {
        func_ov072_02120d04(this);
    } else {
        if (mCachedActor == 0)
            mCachedActor = (s32)FindWithActorID(0x101, 0);
        unk_36c = 0x384;
        func_ov072_02121cdc(this);
        func_ov072_021210c4(this);
    }
    return 1;
}
