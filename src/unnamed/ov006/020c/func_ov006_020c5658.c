typedef struct { int x, y, z; } Vec3;
extern int _Z14ApproachLinearRiii(int* v, int target, int step);
extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern void func_0203ce80(Vec3* dst, Vec3* src);
extern void Vec3_MulScalarInPlace(int* v, int s);
extern void _Z14ApproachLinearRsss(short* v, short target, short step);
extern void func_ov006_020c49d8(void* c);

void func_ov006_020c5658(char* c)
{
    Vec3 sp0, spC, sp18, sp24;
    int r5, r0v;
    int t;

    t = *(int*)(c + 0xa8);
    r5 = _Z14ApproachLinearRiii((int*)(c + 0x9c), *(int*)(c + 0xb4), t < 0 ? -t : t);
    t = *(int*)(c + 0xac);
    r0v = _Z14ApproachLinearRiii((int*)(c + 0xa0), *(int*)(c + 0xb8), t < 0 ? -t : t);

    if (r5 != 0 && r0v != 0 && *(int*)(c + 0xb8) == *(int*)(c + 0x18)) {
        *(int*)(c + 0xb4) = *(int*)(c + 0x14);
        *(int*)(c + 0xb8) = *(int*)(c + 0x1c);
        Vec3_Sub(&sp0, (Vec3*)(c + 0xb4), (Vec3*)(c + 0x9c));
        *(int*)(c + 0xa8) = sp0.x;
        *(int*)(c + 0xac) = sp0.y;
        *(int*)(c + 0xb0) = sp0.z;
        func_0203ce80(&spC, (Vec3*)(c + 0xa8));
        Vec3_MulScalarInPlace((int*)(c + 0xa8), *(int*)(c + 0xdc));
        *(short*)(c + 0xea) = 0;
    } else if (r5 != 0 && r0v != 0 && *(int*)(c + 0xb8) == *(int*)(c + 0x1c)) {
        *(int*)(c + 0xb4) = *(int*)(c + 0x10);
        *(int*)(c + 0xb8) = *(int*)(c + 0x18);
        Vec3_Sub(&sp18, (Vec3*)(c + 0xb4), (Vec3*)(c + 0x9c));
        *(int*)(c + 0xa8) = sp18.x;
        *(int*)(c + 0xac) = sp18.y;
        *(int*)(c + 0xb0) = sp18.z;
        func_0203ce80(&sp24, (Vec3*)(c + 0xa8));
        Vec3_MulScalarInPlace((int*)(c + 0xa8), *(int*)(c + 0xdc));
        *(short*)(c + 0xea) = 0;
    }

    if (*(int*)(c + 0xa8) > 0)
        _Z14ApproachLinearRsss((short*)(c + 0xe6), 0x3000, 0x200);
    else
        _Z14ApproachLinearRsss((short*)(c + 0xe6), -0x3000, 0x200);

    func_ov006_020c49d8(c);
}
