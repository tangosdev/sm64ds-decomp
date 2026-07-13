typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

typedef struct { s32 x, y, z; } Vec3;

extern void func_ov025_02111344(char *t);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(char *self, s32 a, s32 b);
extern void func_ov025_021112e0(char *self);
extern void func_02012694(s32 a, void *b);
extern void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(char *self, Vec3 *pos, s32 fix);
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 slot, u32 effect, s32 x, s32 y, s32 z, const void *rot, void *cb);
extern int func_ov025_0211123c(char *c);
extern s16 data_02082214[];

#define LAUNDER(p) ((long long)(int)(p) & 0xFFFFFFFFFFFFFFFFLL)

int func_ov025_021113f0(char *self)
{
    s32 loc[6];
    s32 n;
    s32 m;

    if (*(s8 *)(self + 0x328) == -1) {
        if (*(u16 *)(self + 0x326) == 0x20) {
            *(u8 *)(self + 0x328) = 0;
            *(u16 *)(self + 0x326) = 0;
        } else {
            u16 *cnt = (u16 *)LAUNDER(self + 0x326);
            (*cnt)++;
            *(s32 *)(self + 0xac) = 0;
            *(u16 *)(self + 0x324) = 0;
            func_ov025_02111344(self);
            if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(self, 0, 0)) {
                func_ov025_021112e0(self);
            }
            return 1;
        }
    }

    n = 10 - *(s8 *)(self + 0x328);
    if (n < 0) {
        s32 neg = -1;
        n = n * neg;
    }
    n = n - 6;
    if (n < 0)
        n = 0;

    if (*(u16 *)(self + 0x326) == n + 8) {
        s8 *p;
        char *b;
        *(u16 *)(self + 0x326) = 0;
        b = self + 0x300;
        p = (s8 *)LAUNDER(self + 0x328);
        *p = (s8)(*p + 1);
        if (*(s8 *)(b + 0x28) == 0x14) {
            u8 *pd = (u8 *)LAUNDER(self + 0x329);
            *pd = (u8)(*pd ^ 1);
            *(s8 *)(self + 0x328) = -1;
        }
    }

    if ((u32)(n - 3) <= 1u) {
        n = 4;
    } else if ((u32)(n - 1) <= 1u) {
        n = 2;
    } else if (n == 0) {
        n = 1;
    }

    m = n << 3;

    if (*(u16 *)(self + 0x326) < m) {
        if (*(u8 *)(self + 0x329) == 0) {
            *(s32 *)(self + 0xac) = 0x14000 / n;
            *(s16 *)(self + 0x324) = (s16)(0x400 / n);
        } else {
            *(s32 *)(self + 0xac) = (-0x14000) / n;
            *(s16 *)(self + 0x324) = (s16)((-0x400) / n);
        }

        {
            s32 *pz = (s32 *)LAUNDER(self + 0x64);
            s16 *pr = (s16 *)LAUNDER(self + 0x8c);
            *pz = *pz + *(s32 *)(self + 0xac);
            *pr = (s16)(*pr + *(s16 *)((self + 0x300) + 0x24));
            if ((*(s16 *)(self + 0x8c) & 0x1fff) < 0x320) {
                if (*(s16 *)(self + 0x324) != 0)
                    func_02012694(0x65, self + 0x74);
            }
        }

        {
            s16 t = *(s16 *)(self + 0x8c);
            s32 idx = ((u16)(s16)(t << 2) >> 4) * 2;
            s32 prod = (s32)data_02082214[idx] * 23;
            if (prod < 0) {
                s32 neg = -1;
                prod = prod * neg;
            }
            *(s32 *)(self + 0x60) = *(s32 *)(self + 0x320) + prod;
        }

        if (*(u16 *)(self + 0x326) == m - 1) {
            loc[3] = *(s32 *)(self + 0x5c);
            loc[4] = *(s32 *)(self + 0x60);
            loc[5] = *(s32 *)(self + 0x64);
            _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(self, (Vec3 *)&loc[3], 0x3e8000);
        }

        loc[0] = *(s32 *)(self + 0x5c);
        loc[1] = *(s32 *)(self + 0x60);
        loc[2] = *(s32 *)(self + 0x64);
        {
            s32 scale = 100;
            s32 zero = 0;
            s32 c;
            c = data_02082214[(*(u16 *)(self + 0x8e) >> 4) * 2 + 1];
            loc[0] = c * scale + *(s32 *)(self + 0x5c);
            loc[1] = *(s32 *)(self + 0x320) - 0xb9000;
            c = data_02082214[(*(u16 *)(self + 0x8e) >> 4) * 2];
            loc[2] = c * scale + *(s32 *)(self + 0x64);
            *(u32 *)(self + 0x32c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                *(u32 *)(self + 0x32c), 0x2d, loc[0], loc[1], loc[2], (void *)zero, (void *)zero);
        }
        {
            s32 scale = 100;
            s32 zero = 0;
            s32 c;
            c = data_02082214[(*(u16 *)(self + 0x8e) >> 4) * 2 + 1];
            loc[0] = *(s32 *)(self + 0x5c) - c * scale;
            c = data_02082214[(*(u16 *)(self + 0x8e) >> 4) * 2];
            loc[2] = *(s32 *)(self + 0x64) - c * scale;
            *(u32 *)(self + 0x330) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                *(u32 *)(self + 0x330), 0x2d, loc[0], loc[1], loc[2], (void *)zero, (void *)zero);
        }
    }

    func_ov025_02111344(self);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(self, 0, 0) || func_ov025_0211123c(self)) {
        func_ov025_021112e0(self);
    }

    {
        u16 *cnt = (u16 *)LAUNDER(self + 0x326);
        (*cnt)++;
    }
    return 1;
}
