typedef unsigned int u32;
typedef unsigned char u8;

extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
extern int func_020124c4(int a, int b, int c, int d);
extern int func_ov006_0210fa40(char *c);
extern void func_ov006_02115598(int a, void *v, int c, int d, int e);
extern void func_ov006_02114fb4(int a, int b);

typedef struct { int x, y; } V2;

#pragma opt_strength_reduction off
#pragma opt_common_subs off

#define LN(base, off) (*(int *)((((int)(base)) + (off)) & 0xFFFFFFFFFFFFFFFF))
#define LM(base, off) (*(int *)((((unsigned int)(base)) + (off)) & 0xFFFFFFFFFFFFFFFF))

void func_ov006_0210fb58(char *c)
{
    int ib, ic;

    if (*(u8 *)(c + 0x73) == 1) {
        if (*(u8 *)(c + 0x74) == 1) {
            int flag = 1;
            int ia;
            for (ia = 0; ia < 3; ia++) {
                int r = RandomIntInternal(&data_0209d4b8);
                LN(c + ia * 8, 0x50) += ((((u32)r >> 16) & 0x7fff) * 0x20 >> 15) * 0x10;
                if (*(int *)(c + ia * 8 + 0x50) >= 0x3000)
                    *(int *)(c + ia * 8 + 0x50) = 0x3000;
                else
                    flag = 0;
            }
            if (flag == 1)
                *(u8 *)(c + 0x74) = 0;
        }
        if (*(u8 *)(c + 0x75) == 1) {
            for (ib = 0; ib < 3; ib++) {
                int v = *(int *)(c + ib * 4 + 0x64);
                if (v > 0) {
                    if (v <= 2) {
                        LN(c + ib * 8, 0x50) -= 0x10;
                        if (*(int *)(c + ib * 8 + 0x50) < 0x200)
                            *(int *)(c + ib * 8 + 0x50) = 0x200;
                    } else {
                        int r = RandomIntInternal(&data_0209d4b8);
                        LM(c + ib * 8, 0x50) -= ((((u32)r >> 16) & 0x7fff) * 0x40 >> 15) + 0x32;
                        if (*(int *)(c + ib * 8 + 0x50) <= 0x800) {
                            *(u8 *)(c + ib + 0x70) = 1;
                            *(int *)(c + ib * 8 + 0x50) = 0x800;
                        }
                    }
                }
            }
        }
    }
    for (ic = 0; ic < 3; ic++) {
        int old = *(int *)(c + ic * 8 + 0x38);
        LN(c + ic * 8, 0x38) += *(int *)(c + ic * 8 + 0x50);
        if (*(u8 *)(c + ic + 0x70) == 1 && *(u8 *)(c + 0x75) == 1) {
            if (old / 0x10000 != *(int *)(c + ic * 8 + 0x38) / 0x10000) {
                if (*(int *)(c + ic * 4 + 0x64) > 0) {
                    LN(c + ic * 4, 0x64) -= 1;
                    if (*(int *)(c + ic * 4 + 0x64) == 0) {
                        *(int *)(c + ic * 8 + 0x50) = 0;
                        *(int *)(c + ic * 8 + 0x38) = (*(int *)(c + ic * 8 + 0x38) / 0x10000) << 16;
                        _ZN5Sound12PlayBank2_2DEj(0x19e);
                    }
                }
            }
        }
        if (*(int *)(c + ic * 8 + 0x38) >= 0x30000)
            LM(c + ic * 8, 0x38) -= 0x30000;
    }
    if (func_ov006_0210fa40(c) == 0) {
        *(int *)(c + 0x84) = func_020124c4(*(int *)(c + 0x84), 2, 0x1a6, 0);
    } else {
        *(int *)(c + 0x84) = 0;
    }
    if (*(u8 *)(c + 0x75) != 1) return;
    if (*(u8 *)(c + 0x73) != 1) return;
    if (func_ov006_0210fa40(c) != 1) return;
    *(u8 *)(c + 0x75) = 0;
    *(u8 *)(c + 0x73) = 0;
    {
        int first = *(int *)(c + 0x38);
        int eq = 1;
        int j;
        for (j = 1; j < 3; j++) {
            if (first != *(int *)(c + j * 8 + 0x38)) {
                eq = 0;
                break;
            }
        }
        if (eq == 1) {
            switch (first) {
            case 0: {
                V2 a;
                a.x = *(int *)(c + 8);
                a.y = *(int *)(c + 0xc);
                func_ov006_02115598(*(int *)(c + 4), &a, 0x1388, 0, 1);
                func_ov006_02114fb4(*(int *)(c + 4), 0);
                break;
            }
            case 0x10000: {
                V2 b;
                b.x = *(int *)(c + 8);
                b.y = *(int *)(c + 0xc);
                func_ov006_02115598(*(int *)(c + 4), &b, 0x1f40, 0, 1);
                func_ov006_02114fb4(*(int *)(c + 4), 1);
                break;
            }
            case 0x20000: {
                V2 d;
                d.x = *(int *)(c + 8);
                d.y = *(int *)(c + 0xc);
                func_ov006_02115598(*(int *)(c + 4), &d, 0x2710, 0, 1);
                func_ov006_02114fb4(*(int *)(c + 4), 2);
                break;
            }
            }
            _ZN5Sound12PlayBank2_2DEj(0x1a7);
        }
    }
    *(int *)(c + 0x7c) = 0x3c;
}
