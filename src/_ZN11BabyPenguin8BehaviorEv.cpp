//cpp
// @symbol _ZN11BabyPenguin8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "BabyPenguin.h"
extern "C" {
int _ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(void* self, int d);
int _ZNK12WithMeshClsn10IsOnGroundEv(void* self);
void func_ov072_02120d04(void* c);
void* _ZN5Actor15FindWithActorIDEjPS_(unsigned int id, void* prev);
void func_ov072_02121cdc(void* c);
void func_ov072_021210c4(void* c);
}

int BabyPenguin::Behavior()
{
    if (_ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(((char*)this), 0x7d0000) && _ZNK12WithMeshClsn10IsOnGroundEv((char*)&mWithMeshClsn)) {
        func_ov072_02120d04(((char*)this));
    } else {
        if (unk_364 == 0)
            *(void**)((char*)&unk_364) = _ZN5Actor15FindWithActorIDEjPS_(0x101, 0);
        unk_36c = 0x384;
        func_ov072_02121cdc(((char*)this));
        func_ov072_021210c4(((char*)this));
    }
    return 1;
}
