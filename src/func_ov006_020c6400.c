typedef struct { int x, y, z; } Vec3;

extern void Vec3_Sub(Vec3 *out, Vec3 *a, Vec3 *b);
extern void Vec3_MulScalar(Vec3 *out, const Vec3 *in, int scale);
extern void AddVec3(Vec3 *a, Vec3 *b, Vec3 *c);
extern void _Z14ApproachLinearRsss(short *p, short b, short c);
extern void func_ov006_020c4d20(char *self);

void func_ov006_020c6400(char *self)
{
    Vec3 diff, scaled;
    int x, y;
    {
        short *pa = (short*)(((long long)(int)(self+0xea)));
        *pa += 0x200;
    }
    Vec3_Sub(&diff, (Vec3*)(self+0xb4), (Vec3*)(self+0x9c));
    Vec3_MulScalar(&scaled, &diff, 0x100);
    *(int*)(self+0xa8) = scaled.x;
    *(int*)(self+0xac) = scaled.y;
    *(int*)(self+0xb0) = scaled.z;
    AddVec3((Vec3*)(self+0x9c), (Vec3*)(self+0xa8), (Vec3*)(self+0x9c));
    if (*(int*)(self+0xa8) > 0)
        _Z14ApproachLinearRsss((short*)(self+0xe6), 0x3000, 0x200);
    else
        _Z14ApproachLinearRsss((short*)(self+0xe6), -0x3000, 0x200);
    x = *(int*)(self+0xa8);
    if (x < 0) x = -x;
    if (x >= 0x20) return;
    y = *(int*)(self+0xac);
    if (y < 0) y = -y;
    if (y >= 0x20) return;
    func_ov006_020c4d20(self);
}
