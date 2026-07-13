typedef int fx32;
typedef long long fx64;

#define FX(a, b) ((fx32)(((fx64)(a) * (b) + 0x800) >> 12))
#define PTR(base, off) ((fx32*)(int)(((long long)(int)((char*)(base) + (off))) & 0xFFFFFFFFFFFFFFFFLL))

void func_02047218(fx32 *m0, fx32 *m1, fx32 *dst, fx32 t)
{
    fx32 s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
    fx32 *p;

    s1  = FX(m0[1], t);
    s2  = FX(m0[2], t);
    s4  = FX(m0[4], t);
    s5  = FX(m0[5], t);
    s7  = FX(m0[7], t);
    s8  = FX(m0[8], t);
    s9  = FX(m0[9], t);
    s10 = FX(m0[10], t);
    s11 = FX(m0[11], t);
    s3  = FX(m0[3], t);
    s6  = FX(m0[6], t);
    s0  = FX(m0[0], t);

    dst[0] += FX(m1[0], s0) + (FX(m1[2], s6) + FX(m1[1], s3));
    p = PTR(dst, 0xc);
    *p += FX(m1[3], s0) + (FX(m1[5], s6) + FX(m1[4], s3));
    p = PTR(dst, 0x18);
    *p += FX(m1[6], s0) + (FX(m1[8], s6) + FX(m1[7], s3));
    p = PTR(dst, 0x24);
    *p += s9 + (FX(m1[9], s0) + (FX(m1[11], s6) + FX(m1[10], s3)));
    p = PTR(dst, 0x4);
    *p += FX(m1[0], s1) + (FX(m1[2], s7) + FX(m1[1], s4));
    p = PTR(dst, 0x10);
    *p += FX(m1[3], s1) + (FX(m1[5], s7) + FX(m1[4], s4));
    p = PTR(dst, 0x1c);
    *p += FX(m1[6], s1) + (FX(m1[8], s7) + FX(m1[7], s4));
    p = PTR(dst, 0x28);
    *p += s10 + (FX(m1[9], s1) + (FX(m1[11], s7) + FX(m1[10], s4)));
    p = PTR(dst, 0x8);
    *p += FX(m1[0], s2) + (FX(m1[2], s8) + FX(m1[1], s5));
    p = PTR(dst, 0x14);
    *p += FX(m1[3], s2) + (FX(m1[5], s8) + FX(m1[4], s5));
    p = PTR(dst, 0x20);
    *p += FX(m1[6], s2) + (FX(m1[8], s8) + FX(m1[7], s5));
    p = PTR(dst, 0x2c);
    *p += s11 + (FX(m1[9], s2) + (FX(m1[11], s8) + FX(m1[10], s5)));
}
