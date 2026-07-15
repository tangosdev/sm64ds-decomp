typedef unsigned int u32;
typedef unsigned char u8;

typedef struct { int x, y, z; } Vec3;
typedef struct { int m[12]; } Mtx;

extern char *_ZN5Actor13ClosestPlayerEv(char *self);
extern void Vec3_Asr(Vec3 *d, Vec3 *s, int sh);
extern void MulVec3Mat4x3(const Vec3 *v, void *m, Vec3 *out);
extern void func_ov080_02125de0(char *c, int a, int b, int d);
extern void func_02012694(u32 id, void *pos);
extern Mtx data_020a0e68;

void func_ov080_021256f8(char *r4)
{
    Vec3 a;
    Vec3 b;
    Vec3 ob;
    Vec3 oa;
    char *r5;
    int za;
    int zb;
    int x, y;
    int lim;
    u32 raw;
    u8 nib;

    r5 = _ZN5Actor13ClosestPlayerEv(r4);
    if (r5 == 0)
        return;

    Vec3_Asr(&a, (Vec3 *)(r5 + 0x68), 3);
    Vec3_Asr(&b, (Vec3 *)(r5 + 0x5c), 3);
    data_020a0e68 = *(Mtx *)(r4 + 0x104);
    MulVec3Mat4x3(&a, &data_020a0e68, &oa);
    MulVec3Mat4x3(&b, &data_020a0e68, &ob);

    za = oa.z;
    if (za <= 0)
        goto L9c;
    zb = ob.z;
    if (zb <= 0)
        goto Lb0;
L9c:
    if (za >= 0)
        goto neg;
    zb = ob.z;
    if (zb < 0)
        goto neg;
Lb0:
    x = ob.x << 3;
    y = ob.y << 3;
    ob.x = x;
    ob.y = y;
    if (x < 0)
        return;
    raw = *(u32 *)(r4 + 8);
    nib = raw & 0xf;
    lim = (nib + 1) * 0x64000;
    if (x > lim)
        return;
    if (y < 0)
        return;
    nib = (raw >> 4) & 0xf;
    lim = (nib + 1) * 0x64000;
    if (y > lim)
        return;
    func_ov080_02125de0(r4, x, y, 1);
    return;

neg:
    if (za <= 0xa000)
        return;
    zb = ob.z;
    if (zb > 0xa000)
        return;
    if (*(unsigned short *)(r4 + 0x1b6) != 0)
        return;
    x = ob.x << 3;
    y = ob.y << 3;
    ob.x = x;
    ob.y = y;
    if (x < -0x46000)
        return;
    raw = *(u32 *)(r4 + 8);
    nib = raw & 0xf;
    lim = (nib + 1) * 0x64000 + 0x46000;
    if (x > lim)
        return;
    if (y < -0xc8000)
        return;
    nib = (raw >> 4) & 0xf;
    lim = (nib + 1) * 0x64000 + 0xc8000;
    if (y > lim)
        return;
    func_ov080_02125de0(r4, x, y, 0);
    func_02012694(0x7b, r4 + 0x74);
}
