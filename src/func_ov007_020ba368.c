#include "types.h"
extern u8 data_ov007_02104ba0[];
extern u8 data_ov007_02104b9c[];
extern s16 data_ov007_02104b98[];
extern u8 data_ov007_0210342c[];
extern short data_02082214[];

extern int _ZN4cstd3divEii(int, int);
extern int _ZN4cstd3modEii(int, int);
extern void func_ov007_020bdeb0(int a);
extern void func_ov007_020bda8c(int a, int b);
extern void func_ov007_020bfe4c(void *o, int x, int y, int z, void *out);
extern void func_ov007_020ba9bc(void);
extern int func_ov007_020c5854(int s, void *v, int a, int b, int c, int d);
extern void func_ov007_020c55bc(int a, int b, int c);

int func_ov007_020ba368(int t)
{
    int s;
    int done = 0;
    int state;
    int local[3];

    {
        char *m = *(char **)data_ov007_02104ba0;
        s = *(int *)m;
        *(int *)(m + 0x1c) = 0;
    }

    if (t == 0) {
        int *p = (int *)(((s64)(int)(*(char **)data_ov007_02104ba0 + 0x20)));
        *p += 1;
        if (*(int *)(*(char **)data_ov007_02104ba0 + 0x20) >= 5) {
            *(int *)(*(char **)data_ov007_02104ba0 + 0x20) = 0;
        }
    }

    if (*(int *)(s + 8) == 0 || *(int *)(*(char **)data_ov007_02104ba0 + 0x14) == 0) {
        int ratio;
        if (t <= 0) {
            ratio = 0;
        } else if (t >= 0x1e) {
            ratio = 0x1000;
        } else {
            ratio = _ZN4cstd3divEii(t << 0xc, 0x1e);
        }
        *(int *)(*(char **)(*(char **)data_ov007_02104b9c + 8) + 0x50) = ratio * 0x1f;
        return ratio >= 0x1000;
    }

    state = *(int *)(*(char **)data_ov007_02104ba0 + 0x20);
    switch (state) {
    case 0:
    case 3:
    {
        int *p1 = (state == 0) ? (int *)(s + 0xb8) : (int *)(s + 0xbc);
        int *p2 = (state == 0) ? (int *)(s + 0xc4) : (int *)(s + 0xc8);
        int d, ratio, ease, v1, v2;
        v1 = *p1;
        v2 = *p2;
        d = v2 - v1;
        if (t <= 0) {
            ratio = 0;
        } else if (t >= 0x28) {
            ratio = 0x1000;
        } else {
            ratio = _ZN4cstd3divEii(t << 0xc, 0x28);
        }
        ease = (int)(((s64)ratio * (0x2000 - ratio)) >> 0xc);
        *(int *)(*(char **)data_ov007_02104ba0 + 0x1c) = 1;
        if (t == 0) {
            *(int *)(*(char **)(*(char **)data_ov007_02104b9c + 8) + 0x50) = 0x1f000;
            func_ov007_020bdeb0(0x1b);
        }
        *p2 = d / 2 - (int)(((s64)ease * d + 0x800) >> 0xc);
        *p1 = *p2 - d * 2;
        func_ov007_020ba9bc();
        *p1 = v1;
        *p2 = v2;
        *(int *)(*(char **)data_ov007_02104b9c) = ease;
        done = ratio >= 0x1000;
        break;
    }
    case 1:
    case 4:
    {
        int r8arg, v, ratio;
        if (t <= 0) {
            ratio = 0;
        } else if (t >= 0x78) {
            ratio = 0x1000;
        } else {
            ratio = _ZN4cstd3divEii(t << 0xc, 0x78);
        }
        if (t == 0) {
            *(int *)(*(char **)data_ov007_02104b9c) = 0x1000;
            *(s16 *)data_ov007_02104b98 = -1;
        }
        if (t >= 0x96) {
            *(int *)(*(char **)data_ov007_02104ba0 + 0x1c) = 6;
        } else {
            *(int *)(*(char **)data_ov007_02104ba0 + 0x1c) = 0;
        }
        {
            char *o = *(char **)(*(char **)data_ov007_0210342c + 0x30);
            func_ov007_020bfe4c(o, 0xe8000, 0xa8000, -*(int *)(o + 0x2c), local);
        }
        func_ov007_020ba9bc();
        if (t <= 0xc8) {
            v = 0;
        } else if (t >= 0x258) {
            v = 0x1000;
        } else {
            v = _ZN4cstd3divEii((t - 0xc8) << 0xc, 0x190);
        }
        ratio += (int)(((s64)v * 0x1e000 + 0x800) >> 0xc);
        r8arg = 0x1000;
        if (*(int *)(*(char **)data_ov007_02104ba0 + 0x20) == 4) {
            if (t <= 0xc8) {
                if (t <= 0x64) {
                    ratio = (int)(((s64)ratio * 0x258 + 0x800) >> 0xc);
                    r8arg = -0xa;
                } else {
                    ratio = (int)(((s64)ratio * 0x320 + 0x800) >> 0xc);
                    r8arg = -0x32;
                }
            } else if (t <= 0x12c) {
                ratio = (int)(((s64)ratio * 0xa + 0x800) >> 0xc);
            }
        }
        {
            int ret = func_ov007_020c5854(s, local, (ratio * 0xc8) >> 0xc, 0x2000, ratio * 0x12c, r8arg);
            int dv = _ZN4cstd3divEii(ret << 0xc, *(int *)(*(char **)data_ov007_02104ba0 + 0x14));
            *(int *)(*(char **)(*(char **)data_ov007_02104b9c + 8) + 0x50) = dv * 0x1f;
            if (dv >= 0x1000) {
                s16 c = *(s16 *)data_ov007_02104b98;
                if (c < 0) {
                    *(s16 *)data_ov007_02104b98 = 0;
                    func_ov007_020bdeb0(0x1e);
                } else {
                    *(s16 *)data_ov007_02104b98 = c + 1;
                    if (*(s16 *)data_ov007_02104b98 >= 0x1e) {
                        done = 1;
                    }
                }
                func_ov007_020bda8c(*(s16 *)data_ov007_02104b98, 1);
            } else {
                func_ov007_020bda8c(t, 0);
            }
        }
        break;
    }
    case 2:
    {
        int a = 0x1000;
        int b = 0x1000;
        if (t == 0) {
            *(int *)(*(char **)data_ov007_02104b9c) = 0x1000;
        }
        if (t <= 0x50) {
            int ratio, m, mc, r2c, ease1, ease2, term, sinval;
            u32 u;
            int idx;
            short tblval;
            if (t <= 0) {
                ratio = 0;
            } else if (t >= 0x28) {
                ratio = 0x1000;
            } else {
                ratio = _ZN4cstd3divEii(t << 0xc, 0x28);
            }
            ease1 = (int)(((s64)ratio * (0x2000 - ratio)) >> 0xc);
            m = _ZN4cstd3modEii(t, 6);
            if (m <= 0) {
                mc = 0;
            } else if (m >= 6) {
                mc = 0x1000;
            } else {
                mc = _ZN4cstd3divEii(m << 0xc, 6);
            }
            if (t <= 0x14) {
                r2c = 0;
            } else if (t >= 0x3c) {
                r2c = 0x1000;
            } else {
                r2c = _ZN4cstd3divEii((t - 0x14) << 0xc, 0x28);
            }
            ease2 = (int)(((s64)r2c * (0x2000 - r2c)) >> 0xc);
            u = (u32)(mc * 0xffff);
            u = u << 4;
            u = u >> 0x10;
            idx = (int)u >> 4;
            tblval = data_02082214[idx * 2];
            term = (int)(((s64)ease2 * 0x3e8 + 0x800) >> 0xc);
            sinval = (int)(((s64)tblval * term + 0x800) >> 0xc);
            if (t <= 0x4d) {
                sinval += (int)(((s64)ease1 * 0x4b0 + 0x800) >> 0xc);
                a += sinval;
                b += sinval;
            } else {
                a = 0x2000;
                b = 0x2000;
            }
            *(int *)(*(char **)data_ov007_02104ba0 + 0x1c) = 2;
        } else {
            int v;
            if (t <= 0x50) {
                v = 0;
            } else if (t >= 0x64) {
                v = 0x1000;
            } else {
                v = _ZN4cstd3divEii((t - 0x50) << 0xc, 0x14);
            }
            a -= v;
            b -= v;
            if (t == 0x51) {
                func_ov007_020bdeb0(0x1f);
            }
            *(int *)(*(char **)data_ov007_02104ba0 + 0x1c) = 5;
            *(int *)(*(char **)(*(char **)data_ov007_02104b9c + 8) + 0x50) = v * 0x1f;
            if (v >= 0x1000) {
                done = 1;
            }
        }
        func_ov007_020c55bc(s, a, b);
        func_ov007_020ba9bc();
        break;
    }
    }
    return done;
}
