#pragma exceptions off

/* storage 0x02384664 runtime 0x037FC4FC size 0x2EC wram ARM.
 * Pools: =chans_38075C4, =arr_3805860, =0xFFFFFD2D, =0xFFFC.
 * Calls fn_02383520, fn_02383E00, fn_02383D9C, fn_02383D00, fn_023838C8,
 * fn_02383950, fn_02384518 plus indirect fp(v4, k, v4->f4C). */

struct Chan;

extern int fn_02383520(int);
extern int fn_02383E00(struct Chan *, int);
extern int fn_02383D9C(struct Chan *, int);
extern int fn_02383D00(struct Chan *, int);
extern int fn_023838C8(int);
extern int fn_02383950(int, int);
extern int fn_02384518(struct Chan *);

typedef void (*FP)(struct Chan *, int, int);

struct Chan {
    unsigned char f00, f01, f02;
    unsigned char f0 : 1, f1 : 1, : 0;   /* 0x03 flags */
    unsigned char f04;
    unsigned char f05, f06, f07, f08, f09;
    signed char f0A, f0B;
    short f0C, f0E;
    unsigned char pad10[0x12];
    unsigned char f22, f23;
    unsigned short f24, f26;
    unsigned char f28;
    unsigned char pad29[0xB];
    int f34;
    unsigned char pad38[4];
    unsigned short f3C;
    unsigned char pad3E[6];
    int f44;
    FP f48;
    int f4C;
    unsigned char pad50[4];
};

extern struct Chan chans_38075C4[];
extern short arr_3805860[];

void fn_02384664(int a1) {
    struct Chan *v4;
    int i;
    int v3, v9, v10, v11, v12, v18;
    for (i = 0; i < 16; ++i) {
        v10 = v3;
        v9 = 0;
        v10 = v9;
        v3 = v9;
        v4 = &chans_38075C4[i];
        if (v4->f0 == 0)
            continue;
        if (v4->f1 != 0) {
            v4->f04 |= 1;
            v4->f1 = 0;
        } else if (fn_02383520(i) == 0) {
            if (v4->f48 != 0)
                v4->f48(v4, 2, v4->f4C);
            else
                v4->f22 = 0;
            v4->f24 = 0;
            v4->f0 = 0;
            continue;
        }
        v9 += arr_3805860[v4->f09];
        v10 += (v4->f08 - v4->f05) << 6;
        v9 += fn_02383E00(v4, a1);
        v10 += fn_02383D9C(v4, a1);
        v11 = fn_02383D00(v4, a1);
        v12 = v4->f28;
        if (v12 == 0)
            goto L0;
        if (v12 == 1)
            goto L1;
        if (v12 == 2)
            v3 += v11;
        goto Ld;
L1:     v9 += v11;
        goto Ld;
L0:     v10 += v11;
Ld:;
        v9 += v4->f0C;
        v10 += v4->f0E;
        v18 = v3 + v4->f0B;
        v3 = v18 + v4->f0A;
        if (v4->f02 == 3 && v9 <= -723) {
            v4->f04 = 2;
            if (v4->f48 != 0)
                v4->f48(v4, 2, v4->f4C);
            else
                v4->f22 = 0;
            v4->f24 = 0;
            v4->f0 = 0;
            continue;
        }
        v9 = fn_023838C8(v9);
        v18 = fn_02383950(v4->f3C, v10);
        if (v4->f01 == 1)
            v18 = (unsigned short)(v18 & 0xFFFC);
        v3 += 64;
        if (v3 < 0)
            v3 = 0;
        else if (v3 > 127)
            v3 = 127;
        if (v9 != v4->f24) {
            v4->f24 = v9;
            v4->f04 |= 8;
        }
        if (v18 != v4->f26) {
            v4->f26 = v18;
            v4->f04 |= 4;
        }
        if (v3 != v4->f23) {
            v4->f23 = v3;
            v4->f04 |= 0x10;
        }
        if (a1 != 0) {
            if (v4->f34 > 0) {
                int result = --v4->f34;
                if (result == 0) {
                    if (v4->f48 != 0)
                        v4->f48(v4, 1, v4->f4C);
                    else
                        v4->f22 = 1;
                    fn_02384518(v4);
                }
            }
        }
    }
}
