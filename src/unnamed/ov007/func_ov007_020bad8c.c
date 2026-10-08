#include "types.h"
extern u8 data_ov007_02104ba0[];
extern u16 data_ov007_020d7b54[];
extern short data_02082214[];

extern void func_ov007_020c92d0(void *a);
extern int _ZN4cstd3modEii(int, int);
extern int _ZN4cstd3divEii(int, int);
extern void func_ov007_020bbc08(int a, int b, int c);
extern void func_ov007_020c55bc(int a, int b, int c);

void func_ov007_020bad8c(void)
{
    char *mgr;
    char *obj;
    int r6;
    int r5;
    u16 period;
    int var_r4;
    int t;
    s16 sw;

    mgr = *(char **)data_ov007_02104ba0;
    r6 = *(int *)mgr;
    func_ov007_020c92d0(*(char **)(mgr + 0xc));

    obj = *(char **)(*(char **)data_ov007_02104ba0 + 0xc);
    r5 = *(int *)(obj + 0xc);
    period = data_ov007_020d7b54[*(s16 *)obj];
    t = _ZN4cstd3modEii(r5, period);
    if (t <= 0) {
        var_r4 = 0;
    } else if (t >= period) {
        var_r4 = 0x1000;
    } else {
        var_r4 = _ZN4cstd3divEii(t << 0xc, period);
    }

    mgr = *(char **)data_ov007_02104ba0;
    obj = *(char **)(mgr + 0xc);
    sw = *(s16 *)obj;

    switch (sw) {
    case 0:
        if (r5 != 0) {
            return;
        }
        func_ov007_020bbc08(1, 0, 0);
        return;
    case 1:
        *(int *)(mgr + 0x1c) = 0;
        if (_ZN4cstd3modEii(r5, 6) != 0) {
            return;
        }
        {
            int v;
            if (var_r4 >= 0x800) {
                v = 0x1000 - ((var_r4 - 0x800) * 2);
            } else {
                v = var_r4 * 2;
            }
            func_ov007_020bbc08(3, 0xa, (int)(((long long)v * (0x2000 - v)) >> 0xc));
        }
        return;
    case 2:
    {
        int v;
        int amt;
        if (var_r4 >= 0x800) {
            v = 0x1000 - ((var_r4 - 0x800) * 2);
        } else {
            v = var_r4 * 2;
        }
        amt = (int)(((long long)v * (0x2000 - v)) >> 0xc) + 0x1000;
        func_ov007_020c55bc(r6, amt, amt);
        obj = *(char **)(*(char **)data_ov007_02104ba0 + 0xc);
        if (r5 >= data_ov007_020d7b54[*(s16 *)obj]) {
            *(s16 *)(obj + 2) = 0;
        }
        if (var_r4 <= 0x800) {
            *(int *)(*(char **)data_ov007_02104ba0 + 0x1c) = 4;
        } else {
            *(int *)(*(char **)data_ov007_02104ba0 + 0x1c) = 0;
        }
        return;
    }
    case 3:
    {
        int d;
        u16 p3;
        int m;
        int var_r0;

        d = r5 - 0x14;
        if (d < 0) {
            return;
        }
        if (d > data_ov007_020d7b54[sw] * 3) {
            return;
        }
        p3 = data_ov007_020d7b54[3];
        m = _ZN4cstd3modEii(d, p3);
        if (m <= 0) {
            var_r0 = 0;
        } else if (m >= p3) {
            var_r0 = 0x1000;
        } else {
            var_r0 = _ZN4cstd3divEii(m << 0xc, p3);
        }
        {
            u32 u = (u32)(var_r0 * 0xffff);
            int idx;
            short tblval;
            int amt;
            u = u << 4;
            u = u >> 0x10;
            idx = (int)u >> 4;
            tblval = data_02082214[idx * 2];
            amt = (int)(((long long)tblval * 0x258 + 0x800) >> 0xc);
            func_ov007_020c55bc(r6, amt + 0x1000, 0x1000 - amt);
        }
        *(int *)(*(char **)data_ov007_02104ba0 + 0x1c) = 2;
        return;
    }
    case 4:
    {
        int m = _ZN4cstd3modEii(r5, 0x3c);
        *(int *)(*(char **)data_ov007_02104ba0 + 0x1c) = 2;
        if (m == 0) {
            func_ov007_020bbc08(1, 0, 0);
            return;
        }
        if (m != 0x1e) {
            return;
        }
        func_ov007_020bbc08(3, 0x1e, (int)(((long long)var_r4 * 0x7d0 + 0x800) >> 0xc) + 0xc8);
        return;
    }
    case 5:
    default:
        return;
    }
}
