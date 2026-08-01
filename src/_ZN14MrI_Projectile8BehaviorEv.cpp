//cpp
// @symbol _ZN14MrI_Projectile8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MrI_Projectile.h"
typedef short s16;

extern "C" void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void* self, const Vector3& v);
extern "C" void Matrix4x3_FromRotationY(void* m, s16 ang);
extern "C" void Matrix4x3_ApplyInPlaceToRotationX(void* m, s16 ang);
extern "C" void MulVec3Mat4x3(void* dst, void* m, void* src);
extern "C" void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void* self, void* clsn);
extern "C" void func_ov071_02121c6c(void* c);
extern "C" void _ZN12CylinderClsn5ClearEv(void* cl);
extern "C" void _ZN12CylinderClsn6UpdateEv(void* cl);
extern "C" unsigned short DecIfAbove0_Short(unsigned short* p);

extern Vector3 data_ov071_021230b8;
extern int data_020a0e68[];

int MrI_Projectile::Behavior()
{
    Vector3 v;
    Vector3 r;
    int flags = unk_0b0;
    int b1 = (int)((flags & 0x40000) != 0);
    int b2;
    if (b1 != 0)
        return 1;
    b2 = (int)((flags & 0x20000) != 0);
    if (b2 == 0) {
        v = data_ov071_021230b8;
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(((char*)this) + 0xfc, v);
        r.z = unk_098;
        r.x = 0;
        r.y = 0;
        Matrix4x3_FromRotationY(data_020a0e68, mPrevAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, mPrevAngleX);
        MulVec3Mat4x3(&r, data_020a0e68, ((char*)this) + 0xa4);
        *(Vector3*)((char*)&unk_0a4) = *(Vector3*)((char*)&unk_0a4);
        _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(((char*)this), ((char*)this) + 0xfc);
        func_ov071_02121b50(((char*)this), ((char*)this) + 0x13c);
        func_ov071_02121ba4(((char*)this));
        func_ov071_02121c6c(((char*)this));
        _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsnWithPos);
        _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsnWithPos);
        if (DecIfAbove0_Short((unsigned short*)((char*)&unk_330)) == 0)
            func_ov071_02121b08(((char*)this));
    } else {
        func_ov071_02121c6c(((char*)this));
    }
    return 1;
}
