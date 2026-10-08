typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int u32;
typedef long long s64;

extern u8 data_ov007_0210342c[];
extern u8 data_ov007_02104ba0[];
extern short data_02082214[];

extern int _ZN4cstd3divEii(int, int);
extern int _ZN4cstd3modEii(int, int);
extern void func_ov007_020bbc08(int a, int b, int c);
extern void func_ov007_020bdeb0(int a);
extern void func_ov007_020bde2c(int idx);
extern void func_ov007_020bda38(int v);
extern void func_020524e4(int *p, int a, int b);
extern void func_ov007_020c5490(char *obj, int *m);
extern void func_ov007_020c55bc(int a, int b, int c);
extern void func_ov007_020c6314(char *c, int i, int b);
extern void func_ov007_020c6174(char *a, int i, int b);

static inline int pressed(char *pad, u16 h, int m1, int m2)
{
    int ok, n3, n2, n5;
    n3 = 0;
    ok = 1;
    n2 = 0;
    n5 = ((h & m1) == 0) ? ok : n3;
    if (n5 != 0) {
        if (*(u16 *)(pad + 2) & m1) {
            n2 = 1;
        }
    }
    if (n2 != 0) {
        if ((h & m2) == 0) {
            n3 = 1;
        }
    }
    if (n3 == 0) {
        if ((h & m2) || !(*(u16 *)(pad + 2) & m2) || n5 == 0) {
            ok = 0;
        }
    }
    return ok;
}

#pragma opt_propagation off
void func_ov007_020bb4b0(void)
{
    int t1;
    int s;
    char *pad;
    int mat[4];
    u16 h;

    s = *(int *)(*(char **)data_ov007_02104ba0);
    pad = *(char **)(*(char **)data_ov007_0210342c + 0x54);

    {
        u32 w = *(u32 *)(pad + 0x28);
        u32 v = *(u32 *)(pad + 0x24);
        if (v <= w) {
            v = w;
        }
        if (_ZN4cstd3modEii(v, 5) == 1) {
            func_ov007_020bbc08(2, 0xa, 0x1000);
            *(int *)(*(char **)data_ov007_02104ba0 + 0x1c) = 0;
        } else {
            h = *(u16 *)pad;
            if (pressed(pad, h, 0x200, 0x100)) {
                func_ov007_020bbc08(1, 0, 0);
                *(int *)(*(char **)data_ov007_02104ba0 + 0x1c) = 3;
                func_ov007_020bdeb0(0x24);
            }
        }
    }

    {
        u32 lim = *(u32 *)(pad + 0x1c);
        int t2 = *(int *)(pad + 8);
        int dir;
        t1 = *(int *)(pad + 4);
        if ((u32)t1 <= lim) {
            t1 = lim;
        }
        lim = *(u32 *)(pad + 0x18);
        if ((u32)t2 <= lim) {
            t2 = lim;
        }
        dir = (t1 > t2) ? 1 : -1;
        if (t1 <= t2) {
            t1 = t2;
        }
        if (t1 > 0) {
            int extra;
            int a;
            int b;
            a = 0x1000;
            extra = 0;
            b = a;
            int ratio, ease, v2, half, fpv;
            *(int *)(*(char **)data_ov007_02104ba0 + 0x1c) = 0;
            if (t1 <= 0) {
                ratio = 0;
            } else if (t1 >= 0x64) {
                ratio = 0x1000;
            } else {
                ratio = _ZN4cstd3divEii(t1 << 0xc, 0x64);
            }
            ease = (int)(((s64)ratio * (0x2000 - ratio)) >> 0xc);
            if (t1 <= 0x50) {
                v2 = 0;
            } else if (t1 >= 0xc8) {
                v2 = 0x1000;
            } else {
                v2 = _ZN4cstd3divEii((t1 - 0x50) << 0xc, 0x78);
            }
            half = ((int)(((s64)v2 * (0x2000 - v2)) >> 0xc)) / 2;
            fpv = half + 0x1000;
            if (t1 >= 0xa0) {
                int m = _ZN4cstd3modEii(t1 - 0xa0, 5);
                int mc, v3, v3sq;
                u32 u;
                s16 tbl;
                if (m <= 0) {
                    mc = 0;
                } else if (m >= 5) {
                    mc = 0x1000;
                } else {
                    mc = _ZN4cstd3divEii(m << 0xc, 5);
                }
                if (t1 <= 0xa0) {
                    v3 = 0;
                } else if (t1 >= 0xc8) {
                    v3 = 0x1000;
                } else {
                    v3 = _ZN4cstd3divEii((t1 - 0xa0) << 0xc, 0x28);
                }
                v3sq = (int)(((s64)v3 * v3) >> 0xc);
                u = (u32)(mc * 0xffff);
                u = u << 4;
                u = u >> 0x10;
                tbl = data_02082214[((int)u >> 4) * 2];
                extra = (int)(((s64)v3sq * (tbl * (s16)0xa) + 0x800) >> 0xc);
            }
            if (dir > 0) {
                a -= ease;
                b += extra + (int)(((s64)ease * fpv + 0x800) >> 0xc);
            } else {
                a += extra + (int)(((s64)ease * fpv + 0x800) >> 0xc);
                b -= ease;
            }
            func_ov007_020c55bc(s, a, b);
            func_ov007_020bde2c(0x38);
        } else {
            h = *(u16 *)pad;
            if (pressed(pad, h, 0x40, 0x400) || pressed(pad, h, 0x80, 2)) {
                func_ov007_020bbc08(0, 0, 0);
                *(int *)(*(char **)data_ov007_02104ba0 + 0x1c) = 3;
                func_ov007_020bdeb0(0x24);
            }
        }
    }

    {
        int c2;
        int c1;
        int dir2;
        int amp;
        u32 lim2;
        lim2 = *(u32 *)(pad + 0x14);
        c1 = *(int *)(pad + 0xc);
        c2 = *(int *)(pad + 0x10);
        if ((u32)c1 <= lim2) {
            c1 = lim2;
        }
        lim2 = *(u32 *)(pad + 0x20);
        if ((u32)c2 <= lim2) {
            c2 = lim2;
        }
        dir2 = (c1 > c2) ? 1 : -1;
        c1 = (c1 > c2) ? c1 : c2;
        if (c1 > 0) {
            int md = _ZN4cstd3modEii(c1, 5);
            if (c1 < 0x64) {
                int r;
                if (c1 <= 0) {
                    r = 0;
                } else if (c1 >= 0x64) {
                    r = 0x1000;
                } else {
                    r = _ZN4cstd3divEii(c1 << 0xc, 0x64);
                }
                amp = r / 2;
            } else {
                int v, sq;
                if (c1 <= 0x64) {
                    v = 0;
                } else if (c1 >= 0x12c) {
                    v = 0x1000;
                } else {
                    v = _ZN4cstd3divEii((c1 - 0x64) << 0xc, 0xc8);
                }
                sq = (int)(((s64)v * v) >> 0xc);
                amp = sq / 2 + 0x800;
                if (c1 >= 0x12c) {
                    int m2 = _ZN4cstd3modEii(c1, 0x64);
                    int mc2;
                    u32 u2;
                    int t2v;
                    if (m2 <= 0) {
                        mc2 = 0;
                    } else if (m2 >= 0x64) {
                        mc2 = 0x1000;
                    } else {
                        mc2 = _ZN4cstd3divEii(m2 << 0xc, 0x64);
                    }
                    u2 = (u32)(mc2 * 0xffff);
                    u2 = u2 << 4;
                    u2 = u2 >> 0x10;
                    t2v = data_02082214[((int)u2 >> 4) * 2];
                    if (t2v < 0) {
                        t2v = -t2v;
                    }
                    amp = (int)(((s64)(t2v / 4 + 0xc00) * amp + 0x800) >> 0xc);
                }
            }
            if (md == 0) {
                u32 uu = (u32)((dir2 * amp) * 0x3555);
                int idx;
                uu = uu << 4;
                uu = uu >> 0x10;
                idx = (int)uu >> 4;
                func_020524e4(mat, data_02082214[idx * 2], data_02082214[idx * 2 + 1]);
                func_ov007_020c5490((char *)s, mat);
            } else {
                int n = _ZN4cstd3divEii(*(int *)(s + 8), 4) + 1;
                if (n == 1) {
                    n = *(int *)(s + 8);
                }
                dir2 = (md - 1) * n;
                md = dir2 + n;
                if (md > *(int *)(s + 8)) {
                    md = *(int *)(s + 8);
                }
                for (; dir2 < md; dir2++) {
                    func_ov007_020c6314(*(char **)(*(char **)data_ov007_02104ba0), dir2, 0);
                    func_ov007_020c6174(*(char **)(*(char **)data_ov007_02104ba0), dir2, 1);
                }
            }
            func_ov007_020bda38(amp);
            *(int *)(*(char **)data_ov007_02104ba0 + 0x1c) = 2;
        } else {
            h = *(u16 *)pad;
            if (pressed(pad, h, 0x10, 1) || pressed(pad, h, 0x20, 0x800)) {
                func_ov007_020bbc08(0, 0, 0);
                *(int *)(*(char **)data_ov007_02104ba0 + 0x1c) = 0;
            }
        }
    }
}
