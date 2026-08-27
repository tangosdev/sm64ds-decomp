//cpp
// @symbol _ZN8daEyBm_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daEyBm_c.h"
typedef short s16;

extern "C" void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void* self, const Vector3& v);
extern "C" void Matrix4x3_FromRotationY(void* m, s16 ang);
extern "C" void Matrix4x3_ApplyInPlaceToRotationX(void* m, s16 ang);
extern "C" void MulVec3Mat4x3(void* dst, void* m, void* src);
extern "C" void _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(void* self, void* clsn);
extern "C" void func_ov071_02121c6c(void* c);
extern "C" void _ZN5dCc_c5ClearEv(void* cl);
extern "C" void _ZN5dCc_c6UpdateEv(void* cl);
extern "C" unsigned short DecIfAbove0_Short(unsigned short* p);

extern Vector3 data_ov071_021230b8;
extern int data_020a0e68[];

int daEyBm_c::Behavior()
{
    Vector3 v;
    Vector3 r;
    int flags = mFlags;
    int b1 = (int)((flags & 0x40000) != 0);
    int b2;
    if (b1 != 0)
        return 1;
    b2 = (int)((flags & 0x20000) != 0);
    if (b2 == 0) {
        v = data_ov071_021230b8;
        _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(((char*)this) + 0xfc, v);
        r.z = mHorzSpeed;
        r.x = 0;
        r.y = 0;
        Matrix4x3_FromRotationY(data_020a0e68, mPrevAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, mPrevAngleX);
        MulVec3Mat4x3(&r, data_020a0e68, ((char*)this) + 0xa4);
        *(Vector3*)((char*)&unk_0a4) = *(Vector3*)((char*)&unk_0a4);
        _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(((char*)this), ((char*)this) + 0xfc);
        func_ov071_02121b50(((char*)this), ((char*)this) + 0x13c);
        func_ov071_02121ba4(((char*)this));
        func_ov071_02121c6c(((char*)this));
        _ZN5dCc_c5ClearEv((char*)&mdCcAcPos_c);
        _ZN5dCc_c6UpdateEv((char*)&mdCcAcPos_c);
        if (DecIfAbove0_Short((unsigned short*)((char*)&mLifeTimer)) == 0)
            func_ov071_02121b08(((char*)this));
    } else {
        func_ov071_02121c6c(((char*)this));
    }
    return 1;
}
