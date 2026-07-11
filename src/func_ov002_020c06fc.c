/* NONMATCHING div=50 (2026-07-11, from scratch): structure byte-aligned
 * (324/368 equal, size exact, frame/pushset exact after wrapping the four
 * Vec3s + ray[0x78] in ONE escaping struct to stop v0 scalarization).
 * Residue: (a) scratch rotation inside the a*0x52 fix12 block (r2/r3, r5/r7,
 * ip/r1), (b) v1/v2 init interleave (scheduler hoists table reads above the
 * v1.x store; ROM value-order z,y,x store-order x,y,z already applied),
 * (c) fx/fz rounding interleave. All scheduling/scratch family.
 * func_ov002_020c06fc @ 0x020c06fc size 0x5c0 (ov002) — swim/current steering:
 * recompute c->98 speed vector from stick + current, returns 0 if idle. */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

struct Vec3 { s32 x, y, z; };

#define FX(a, k) ((int)(((long long)(a) * (k) + 0x800) >> 12))

extern int func_ov002_020c031c(void *c);
extern int func_ov002_020f030c(unsigned int sel);
extern int func_ov002_020f02c8(unsigned int sel);
extern int func_ov002_020f035c(unsigned int sel, int r1);
extern int func_0203cf78(void *v);
extern int func_0203b4dc(int x, int z);
extern int func_0203b0e8(int a, int b);
extern void func_020377b0(void *ray);
extern void func_02037670(void *ray, void *a, void *b, void *actor);
extern int func_02038638(void *ray);
extern void func_02037764(void *ray);
extern void ApproachAngle(short *cur, short target, int divisor, int band, int maxStep);

extern unsigned char data_020a0e40;
extern short data_0209f4a0[];
extern char data_0209f4ac[];
extern short data_02082214[];

int func_ov002_020c06fc(char *c, int arg)
{
    struct Frame {
        struct Vec3 v0;
        struct Vec3 v1;
        struct Vec3 v2;
        struct Vec3 v3;
        char ray[0x78];
    } f;
    int a;
    int t;
    int b;
    int v;
    int spd;
    int r5v;
    int r4v;
    int ang;

    if (*(u8 *)(c + 0x6de) != 0)
        return 0;
    if ((*(int *)(c + 0x558) | *(int *)(c + 0x98)) == 0)
        return 0;

    a = 0;
    b = a;
    if (*(s16 *)((char *)data_0209f4a0 + data_020a0e40 * 0x18) >= 0x200) {
        s16 d = (s16)(*(s16 *)(c + 0x6d2) - *(s16 *)(c + 0x94));
        int j = ((u16)d >> 4) * 2;
        a = data_02082214[j + 1];
        b = data_02082214[j];
    }

    t = func_ov002_020f030c(func_ov002_020c031c(c));
    t = t + FX(FX(a, 0x52LL), *(s16 *)((char *)data_0209f4a0 + data_020a0e40 * 0x18));

    v = func_ov002_020f02c8(func_ov002_020c031c(c));
    {
        int j = (*(u16 *)(c + 0x94) >> 4) * 2;
        spd = *(int *)(c + 0x98);
        r5v = FX(spd, (int)data_02082214[j]);
        r4v = FX(spd, (int)data_02082214[j + 1]);
    }

    if (*(u8 *)(c + 0x70e) != 0) {
        if (*(u8 *)(data_0209f4ac + data_020a0e40 * 0x18) != 0) {
            int m = (b < 0) ? -b : b;
            s16 s2;
            if (m < 0x100)
                b = 0;
            s2 = *(s16 *)((char *)data_0209f4a0 + data_020a0e40 * 0x18);
            if (s2 < 0x400)
                *(u16 *)(c + 0x69c) = FX(b, 0x64LL);
            else if (s2 < 0x800)
                *(u16 *)(c + 0x69c) = FX(b, 0xC8LL);
            else if (s2 < 0xC00)
                *(u16 *)(c + 0x69c) = FX(b, 0x190LL);
            else
                *(u16 *)(c + 0x69c) = FX(b, 0x300LL);
        } else {
            *(u16 *)(c + 0x69c) = FX(b, 0x220LL);
        }
    } else {
        *(u16 *)(c + 0x69c) = (int)((((long long)b << 8) + 0x800) >> 12);
    }

    f.v0.x = FX(v, (int)data_02082214[(*(u16 *)(c + 0x69a) >> 4) * 2]);
    f.v0.z = FX(v, (int)data_02082214[(*(u16 *)(c + 0x69a) >> 4) * 2 + 1]);
    {
        int len = func_0203cf78(c + 0x554);
        f.v0.x = FX(f.v0.x, len);
        f.v0.z = FX(f.v0.z, len);
    }

    if ((*(u8 *)(c + 0x6e9) & 2) != 0 && (u16)(*(u16 *)(c + 0x6ce) & 1) == 0
        && *(int *)(c + 0x98) <= 0x10000) {
        int r6b = func_0203b4dc(f.v0.x, f.v0.z);
        func_020377b0(f.ray);
        {
            int j = ((u16)r6b >> 4) * 2;
            int tz;
            int ty;
            int tx;
            f.v1.y = *(int *)(c + 0x60) + 0x32000;
            f.v1.z = *(int *)(c + 0x64);
            f.v1.x = *(int *)(c + 0x5c);
            tz = data_02082214[j + 1] * 0x46 + *(int *)(c + 0x64);
            ty = *(int *)(c + 0x60) + 0x32000;
            tx = data_02082214[j] * 0x46 + *(int *)(c + 0x5c);
            f.v2.x = tx;
            f.v2.y = ty;
            f.v2.z = tz;
        }
        func_02037670(f.ray, &f.v1, &f.v2, c);
        if (func_02038638(f.ray) != 0) {
            f.v0.x = 0;
            f.v0.z = 0;
        }
        func_02037764(f.ray);
    }

    {
        int fx = FX(t, r5v + f.v0.x);
        int fz = FX(t, r4v + f.v0.z);
        f.v3.x = fx;
        f.v3.z = fz;
        ang = func_0203b4dc(fx, fz);
    }
    *(int *)(c + 0x98) = func_0203cf78(&f.v3);
    if (*(int *)(c + 0x98) > 0x64000)
        *(int *)(c + 0x98) = 0x64000;
    *(s16 *)(c + 0x94) = (s16)(ang + *(s16 *)(c + 0x69c));
    {
        s16 cur = *(s16 *)(c + 0x94);
        if (func_0203b0e8(cur, *(s16 *)(c + 0x8e)) >= 0x4000)
            cur = (s16)(*(s16 *)(c + 0x94) + 0x8000);
        ApproachAngle((s16 *)(c + 0x8e), cur, 8, 0x4000, 0x10);
    }
    {
        int s = *(int *)(c + 0x98);
        if (s < 0)
            s = -s;
        if (s < arg) {
            if (func_ov002_020f035c(func_ov002_020c031c(c), *(int *)(c + 0x558)) == 0) {
                *(int *)(c + 0x98) = 0;
                return 0;
            }
            if (*(u8 *)(c + 0x70e) != 0)
                *(int *)(c + 0x98) = arg;
        }
    }
    return 1;
}
