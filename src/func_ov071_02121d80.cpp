//cpp
typedef short s16;
struct Vector3 { int x, y, z; };

extern "C" void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void* self, const Vector3& v);
extern "C" void Matrix4x3_FromRotationY(void* m, s16 ang);
extern "C" void Matrix4x3_ApplyInPlaceToRotationX(void* m, s16 ang);
extern "C" void MulVec3Mat4x3(void* dst, void* m, void* src);
extern "C" void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void* self, void* clsn);
extern "C" void func_ov071_02121b50(void* self, void* clsn);
extern "C" void func_ov071_02121ba4(void* c);
extern "C" void func_ov071_02121c6c(void* c);
extern "C" void _ZN12CylinderClsn5ClearEv(void* cl);
extern "C" void _ZN12CylinderClsn6UpdateEv(void* cl);
extern "C" unsigned short DecIfAbove0_Short(unsigned short* p);
extern "C" void func_ov071_02121b08(void* c);

extern Vector3 data_ov071_021230b8;
extern int MATRIX_SCRATCH_PAPER[];

extern "C" int func_ov071_02121d80(char* c)
{
    Vector3 v;
    Vector3 r;
    int flags = *(int*)(c + 0xb0);
    int b1 = (int)((flags & 0x40000) != 0);
    int b2;
    if (b1 != 0)
        return 1;
    b2 = (int)((flags & 0x20000) != 0);
    if (b2 == 0) {
        v = data_ov071_021230b8;
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0xfc, v);
        r.z = *(int*)(c + 0x98);
        r.x = 0;
        r.y = 0;
        Matrix4x3_FromRotationY(MATRIX_SCRATCH_PAPER, *(s16*)(c + 0x94));
        Matrix4x3_ApplyInPlaceToRotationX(MATRIX_SCRATCH_PAPER, *(s16*)(c + 0x92));
        MulVec3Mat4x3(&r, MATRIX_SCRATCH_PAPER, c + 0xa4);
        *(Vector3*)(c + 0xa4) = *(Vector3*)(c + 0xa4);
        _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(c, c + 0xfc);
        func_ov071_02121b50(c, c + 0x13c);
        func_ov071_02121ba4(c);
        func_ov071_02121c6c(c);
        _ZN12CylinderClsn5ClearEv(c + 0xfc);
        _ZN12CylinderClsn6UpdateEv(c + 0xfc);
        if (DecIfAbove0_Short((unsigned short*)(c + 0x330)) == 0)
            func_ov071_02121b08(c);
    } else {
        func_ov071_02121c6c(c);
    }
    return 1;
}
