typedef signed char s8;
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int u32;

extern u8 data_ov007_0210342c[];
extern short data_02082214[];

extern void func_ov007_020c92d0(void *a);
extern void func_ov007_020c0638(void *a, int b, int c, int d);
extern void func_ov007_020c02d8(void *a, int b, u16 c);
extern void func_ov007_020bfacc(int a, int b, int c);
extern void func_ov007_020c1d8c(int a);
extern int _ZN4cstd3divEii(int, int);
extern void func_ov007_020ada70(void);

void func_ov007_020ad660(char *arg0)
{
    char *mgr;
    char *temp_r5;
    char *temp_r4;
    char *temp_r2;
    int temp_r1;
    u8 state;

    mgr = *(char **)data_ov007_0210342c;
    temp_r5 = *(char **)(mgr + 4);
    temp_r4 = *(char **)(mgr + 8);

    if (*(int *)(*(char **)(arg0 + 4) + 8) == 0) {
        return;
    }

    func_ov007_020c92d0(*(char **)arg0);

    state = *(u8 *)(arg0 + 8);
    temp_r2 = *(char **)arg0;
    temp_r1 = *(int *)(temp_r2 + 0xc);

    switch (state) {
    case 1:
    case 6:
    {
        char *m134 = *(char **)(*(char **)data_ov007_0210342c + 0x134);
        char *obj = *(char **)m134;
        s16 v = *(s16 *)(*(char **)(obj + 4));
        if ((u32)(u16)(s16)(v - 3) > 1u) {
            return;
        }
        {
            s16 c2 = *(s16 *)(*(char **)(obj + 0x24) + 4);
            int c1 = *(int *)(obj + 0xc) >> 0xc;
            func_ov007_020c0638(*(char **)(arg0 + 4), 0, c1 - c2, 0);
        }
        return;
    }
    case 5:
        if (*(s16 *)temp_r5 != 5) {
            return;
        }
        if (*(int *)(temp_r5 + 0xc) == 0) {
            func_ov007_020c02d8(*(char **)(*(char **)(arg0 + 4) + 0x10), 0x18, 0x300);
        }
        if (*(int *)(*(char **)(*(char **)data_ov007_0210342c + 0x40) + 0xc) != 0) {
            return;
        }
        func_ov007_020bfacc(9, 0, (int)func_ov007_020ada70);
        func_ov007_020c02d8(*(char **)(*(char **)(arg0 + 4) + 0x10), 0x18, 0x301);
        *(int *)(*(char **)(arg0 + 4) + 0xc) = 1;
        func_ov007_020c1d8c(0);
        return;
    case 2:
        switch (*(s16 *)temp_r2) {
        case 0:
        {
            s16 v5;
            s16 t4;
            v5 = *(s16 *)temp_r5;
            if (v5 != 0) {
                return;
            }
            t4 = *(s16 *)temp_r4;
            if (t4 == 0xb && *(int *)(temp_r4 + 0xc) == 0) {
                *(int *)(*(char **)(arg0 + 4) + 4) = 3;
            } else if (t4 == 0xa && *(int *)(temp_r4 + 0xc) == 0) {
                *(int *)(*(char **)(arg0 + 4) + 4) = 2;
                *(s16 *)(*(char **)arg0 + 2) = 1;
            }
            if (*(int *)(*(char **)data_ov007_0210342c + 0xec) != 1) {
                return;
            }
            {
                char *a1 = *(char **)(*(char **)data_ov007_0210342c + 0xf4);
                char *a2 = *(char **)(a1 + 0x18);
                if (*(s16 *)(a2 + 0xc) != 0) {
                    return;
                }
            }
            {
                char *b1 = *(char **)(*(char **)data_ov007_0210342c + 0x104);
                char *b2 = *(char **)(b1 + 4);
                char *b3 = *(char **)(b2 + 0x18);
                u16 v = *(u16 *)(b3 + 0x10);
                if (v == 0x11d || v == 0x12c || v == 0x13d || v == 0x163 || v == 0x178 || v == 0x18f) {
                    *(s16 *)(*(char **)arg0 + 2) = 2;
                }
            }
            return;
        }
        case 1:
        {
            int var_r4;
            if (temp_r1 <= 0) {
                var_r4 = 0;
            } else if (temp_r1 >= 0x50) {
                var_r4 = 0x1000;
            } else {
                var_r4 = _ZN4cstd3divEii(temp_r1 << 0xc, 0x50);
            }
            {
                int t = (int)(((long long)var_r4 * 0xe10 + 0x800) >> 0xc);
                int r = _ZN4cstd3divEii(t << 0x10, 0x168);
                int inv, inv2, prod, result;
                r = (u16)r;
                inv = 0x1000 - var_r4;
                inv2 = (int)(((long long)inv * inv) >> 0xc);
                inv2 = (int)(((long long)inv2 * inv2) >> 0xc);
                if (inv2 < 0x50) {
                    inv2 = 0;
                }
                prod = data_02082214[(r >> 4) * 2] * 0x19;
                result = (int)(((long long)inv2 * prod + 0x800) >> 0xc);
                *(int *)(*(char **)(arg0 + 4) + 0x2c) = result;
            }
            if (var_r4 >= 0x1000) {
                *(s16 *)(*(char **)arg0 + 2) = 0;
            }
            return;
        }
        case 2:
        {
            int var_r0;
            if (temp_r1 <= 0) {
                var_r0 = 0;
            } else if (temp_r1 >= 8) {
                var_r0 = 0x1000;
            } else {
                var_r0 = _ZN4cstd3divEii(temp_r1 << 0xc, 8);
            }
            {
                int inv = 0x1000 - var_r0;
                int sq = (int)(((long long)inv * inv) >> 0xc);
                u32 t = (u32)(var_r0 * 0xffff);
                int idx;
                int half;
                short tblval;
                int temp_a;
                int result;
                t = t << 4;
                t = t >> 0x10;
                idx = (int)t >> 4;
                half = sq / 2;
                tblval = data_02082214[idx * 2];
                temp_a = half + 0x800;
                result = (int)(((long long)temp_a * tblval + 0x800) >> 0xc);
                *(int *)(*(char **)(arg0 + 4) + 0x2c) = -result;
            }
            if (var_r0 >= 0x1000) {
                *(s16 *)(*(char **)arg0 + 2) = 0;
            }
            return;
        }
        default:
            return;
        }
    default:
        return;
    }
}
