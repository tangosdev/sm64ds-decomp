//cpp
// @symbol _ZN10ChainChomp8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ChainChomp.h"
extern "C" {
int func_ov014_02111fb8(char* c);
char* _ZN5Actor15FindWithActorIDEjPS_(unsigned int a, void* b);
void func_ov014_02111f08(char* c);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* cc);
void func_ov014_02112114(char* c);
void func_ov014_02111fe0(char* c);
void func_ov014_0211250c(char* c);
void func_ov014_0211236c(char* c);
void func_ov014_021122dc(char* c);
void func_ov014_02112788(char* c);
void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void* self, void* v);
void _ZN12CylinderClsn5ClearEv(void* self);
char* _ZN5Actor13ClosestPlayerEv(char* self);
void _ZN12CylinderClsn6UpdateEv(void* self);
}

int ChainChomp::Behavior()
{
    unk_61c = 0;
    {
        int v = unk_5f0 + 0xc8000;
        if (mPosY <= v) {
            mPosY = v;
            if (unk_61d == 0) {
                func_ov014_02111fb8(((char*)this));
            }
            unk_61c = 1;
        }
    }
    unk_61d = unk_61c;
    if (unk_60c == 0) {
        char* r = _ZN5Actor15FindWithActorIDEjPS_(0x29, 0);
        unk_60c = *(int*)(r + 4);
    }
    func_ov014_02111f08(((char*)this));
    _ZN5Actor9UpdatePosEP12CylinderClsn(((char*)this), ((char*)this) + 0x110);
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
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(((char*)this) + 0x110, v);
    }
    _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsnWithPos);
    if (*(unsigned char*)(_ZN5Actor13ClosestPlayerEv(((char*)this)) + 0x6fb) == 0) {
        _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsnWithPos);
    }
    return 1;
}
