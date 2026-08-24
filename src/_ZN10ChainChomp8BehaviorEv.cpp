//cpp
// @symbol _ZN10ChainChomp8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ChainChomp.h"
extern "C" {
int func_ov014_02111fb8(char* c);
char* _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int a, void* b);
void func_ov014_02111f08(char* c);
void _ZN8dActor_c9UpdatePosEP5dCc_c(void* self, void* cc);
void func_ov014_02112114(char* c);
void func_ov014_02111fe0(char* c);
void func_ov014_0211250c(char* c);
void func_ov014_0211236c(char* c);
void func_ov014_021122dc(char* c);
void func_ov014_02112788(char* c);
void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void* self, void* v);
void _ZN5dCc_c5ClearEv(void* self);
char* _ZN8dActor_c13ClosestPlayerEv(char* self);
void _ZN5dCc_c6UpdateEv(void* self);
}

int ChainChomp::Behavior()
{
    mIsOnGround = 0;
    {
        int v = mSpawnPosY + 0xc8000;
        if (mPosY <= v) {
            mPosY = v;
            if (mWasOnGround == 0) {
                func_ov014_02111fb8(((char*)this));
            }
            mIsOnGround = 1;
        }
    }
    mWasOnGround = mIsOnGround;
    if (mFenceUniqueID == 0) {
        char* r = _ZN8dActor_c15FindWithActorIDEjPS_(0x29, 0);
        mFenceUniqueID = *(int*)(r + 4);
    }
    func_ov014_02111f08(((char*)this));
    _ZN8dActor_c9UpdatePosEP5dCc_c(((char*)this), &mdCcAcPos_c);
    func_ov014_02112114(((char*)this));
    if (unk_605 == 0) {
        func_ov014_02111fe0(((char*)this));
    }
    func_ov014_0211250c(((char*)this));
    if (unk_605 == 0) {
        func_ov014_0211236c(((char*)this));
        func_ov014_021122dc(((char*)this));
    }
    func_ov014_02112788(((char*)this));
    {
        int v[3];
        v[0] = data_ov014_02114700[0];
        v[1] = data_ov014_02114700[1];
        v[2] = data_ov014_02114700[2];
        _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(&mdCcAcPos_c, v);
    }
    _ZN5dCc_c5ClearEv((char*)&mdCcAcPos_c);
    if (*(unsigned char*)(_ZN8dActor_c13ClosestPlayerEv(((char*)this)) + 0x6fb) == 0) {
        _ZN5dCc_c6UpdateEv((char*)&mdCcAcPos_c);
    }
    return 1;
}
