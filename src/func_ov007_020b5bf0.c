typedef signed char s8;
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int u32;

extern short data_02082214[];
extern u8 data_ov007_0210342c[];

extern int _ZN4cstd3divEii(int, int);
extern unsigned short func_ov007_020c3be0(int t, int a, int b);
extern void func_ov007_020b5f64(void *a);
extern void func_ov007_020c8f98(int a);

void func_ov007_020b5bf0(char *arg0)
{
    char *r7 = *(char **)arg0;
    int r6 = 0;
    int r5 = r6;
    int r4 = r6;

    if (*(u16 *)(*(char **)(r7 + 0x24)) == 0) {
        if (*(int *)(r7 + 0x18) > 0) {
            int a = *(int *)(r7 + 0x18);
            int diff = *(int *)(r7 + 0x1c) - a;
            int sb;
            if (diff <= 0) {
                sb = r6;
            } else if (diff >= *(int *)(r7 + 0x1c)) {
                sb = 0x1000;
            } else {
                sb = _ZN4cstd3divEii(diff << 0xc, *(int *)(r7 + 0x1c));
            }
            {
                int t1 = (sb * (*(int *)(r7 + 0x1c) * 0x48)) >> 0xc;
                int r = _ZN4cstd3divEii(t1 << 0x10, 0x168);
                int idx;
                short tblval;
                int inv;
                u16 typeField;
                int r2;
                int mulfactor;
                int result;

                r = (u16)r;
                idx = (r >> 4) * 2;
                tblval = data_02082214[idx];
                inv = 0x1000 - sb;
                typeField = *(u16 *)r7;
                r2 = (int)(((long long)tblval * inv + 0x800) >> 0xc);
                mulfactor = (typeField == 0) ? 4 : 8;
                result = (r2 * mulfactor) >> 0xc;
                *(u16 *)(*(char **)(arg0 + 4) + 0x38) = _ZN4cstd3divEii(result << 0x10, 0x168);
            }
        } else {
            *(u16 *)(*(char **)(arg0 + 4) + 0x38) = r6;
        }
    }

    {
        u16 switchVal = *(u16 *)(*(char **)(r7 + 0x24));
        int var_r0_2;

        switch (switchVal) {
        case 0:
        case 2:
        case 6:
            r5 = 1;
            break;
        case 4:
        {
            s16 v = *(s16 *)(*(char **)(*(char **)arg0 + 4));
            if ((u32)(u16)(s16)(v - 3) <= 1u) {
                r4 = 1;
            } else {
                r5 = 1;
            }
            break;
        }
        case 8:
            r4 = 1;
            break;
        case 7:
        {
            u16 typeField = *(u16 *)r7;
            char *obj = *(char **)(*(char **)(*(char **)data_ov007_0210342c + 0x28));
            s16 v = *(s16 *)(*(char **)(r7 + 4));

            if (v != 2) goto L_1bc;
            if (typeField == 9) goto L_set;
            if (typeField != 0xc) goto L_1a8;
            if (*(u8 *)(obj + 9) != 0) goto L_set;
        L_1a8:
            if (typeField != 0xd) goto L_1bc;
            if (*(u8 *)(obj + 9) == 0) goto L_set;
        L_1bc:
            if (typeField != 0xc) goto L_1d0;
            if (*(u8 *)(obj + 9) == 0) goto L_set;
        L_1d0:
            if (typeField != 0xd) goto L_1f4;
            if (*(u8 *)(obj + 9) == 0) goto L_1f4;
        L_set:
            {
                char *m4 = *(char **)(arg0 + 4);
                *(u16 *)(m4 + 0x70) = *(u16 *)(m4 + 0xc);
            }
            break;
        L_1f4:
            if (v == 2) break;
            if (typeField == 9) goto L_zero;
            if (typeField != 0xc) goto L_218;
            if (*(u8 *)(obj + 9) != 0) goto L_zero;
        L_218:
            if (typeField != 0xd) break;
            if (*(u8 *)(obj + 9) != 0) break;
        L_zero:
            *(u16 *)(*(char **)(arg0 + 4) + 0x70) = 0;
            break;
        }
        case 3:
        {
            s16 v = *(s16 *)(*(char **)(*(char **)arg0 + 4));
            if ((u32)(u16)(s16)(v - 3) <= 1u) {
                r4 = 1;
            } else {
                r6 = 1;
            }
            break;
        }
        default:
            break;
        }

        var_r0_2 = (r4 != 0) ? (0x1000 - *(int *)(r7 + 0x14)) : *(int *)(r7 + 0x10);

        if (r5 != 0 || r4 != 0) {
            if (var_r0_2 >= 0x1000) {
                *(int *)(*(char **)(arg0 + 4) + 0x50) = 0x1f000;
            } else if (var_r0_2 <= 0) {
                *(int *)(*(char **)(arg0 + 4) + 0x50) = 0;
            } else {
                *(int *)(*(char **)(arg0 + 4) + 0x50) = (int)(((long long)var_r0_2 * 0x1f000 + 0x800) >> 0xc);
            }
        }

        if (r6 != 0) {
            if (var_r0_2 >= 0x1000) {
                *(u16 *)(*(char **)(arg0 + 4) + 0x44) = 0x7fff;
            } else if (var_r0_2 <= 0) {
                *(u16 *)(*(char **)(arg0 + 4) + 0x44) = 0x4210;
            } else {
                *(u16 *)(*(char **)(arg0 + 4) + 0x44) = func_ov007_020c3be0(var_r0_2, 0x4210, 0x7fff);
            }
        }
    }

    if ((u32)(u16)(*(u16 *)r7 + 0xfff2) <= 2u) {
        func_ov007_020b5f64(arg0);
    }
    func_ov007_020c8f98(*(int *)(arg0 + 0x10));
}
