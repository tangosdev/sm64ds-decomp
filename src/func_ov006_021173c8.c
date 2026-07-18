//cpp
// NONMATCHING: 21/1072 words differ (div=21), all mwccarm spill-slot/coloring internals.
// Residual = stack-slot allocation order the compiler picks non-deterministically: loop C's
// val/pcnt land at sp+0x10/0x2c instead of 0x10/0x14 (cascading a 4-byte shift onto the
// hoisted tag slots 0x20-0x3c); loop G's pcnt2 coalesces into val's slot (0x14) instead of
// getting its own 0x1c; loop 2's digit-count reload uses split add-#0x4000 addressing instead
// of the pooled register-offset ldr [r1,r0]. All logic/structure/addressing otherwise exact.
// func_ov006_021173c8 @ 0x021173c8 size 0x10c0 (ov006), mwccarm 1.2/sp2p3.

#pragma opt_common_subs off
#pragma opt_strength_reduction off

struct DispObj {
    virtual void f0();
    virtual void f1();
};

extern "C" {
void func_02018f8c(void *p, int a);
void func_ov004_020afdd0(int a0, int a1, int a2, int a3, int a4);
void func_ov004_020b1ea4(int a0, int a1, int a2, int a3, int a4, int a5, int a6);
int func_ov004_020ad674(void);
int func_ov004_020adbc0(void);
int func_ov004_020adc1c(void);
void func_ov004_020b1a5c(int a0, int a1);
int func_ov004_020b19f0(void *self);
void func_ov004_020b0d8c(void *c, int a1, int a2);
void func_ov006_02114b10(void *self);
void func_ov006_02113c14(void *self);

extern int data_ov006_02138ae0[];
extern char *data_ov006_0213ecb8[];
extern int data_ov006_0212edfc[];
extern int data_ov006_0213765c[];
extern int data_ov006_02137a78[];
extern int data_ov006_02137a6c[];
extern int data_ov006_021382e0[];
extern int data_ov006_02138bd0[];
extern int data_ov006_02138bf4[];
extern int data_ov006_02137a0c[];
extern int data_ov006_02137a60[];
extern int data_ov006_02138a40[];
extern int data_ov006_02138a88[];
}

#define I(off) (*(int *)(g + (off)))
#define UC(off) (*(unsigned char *)(g + (off)))
#define LP(e) ((int *)(((long long)(int)(e)) & 0xFFFFFFFFFFFFFFFFLL))

extern "C" int func_ov006_021173c8(void *this_)
{
    char *g = (char *)this_;
    int col;
    int xpos;
    int r7;
    int limit;
    int i;
    int j;
    int limG;

    func_02018f8c(*(void **)(g + 0x5998), 0);
    col = 0;

    if (I(0x4660) == 1 && I(0x5998) <= 0) {
        xpos = 0x18;

        /* Block B: row 0 */
        if (I(0x5964) > 0) {
            func_ov004_020afdd0(data_ov006_02138ae0[0], 0x30, xpos, -1, -1);
            func_ov004_020b1ea4(0x80, xpos, 0x64, 0, 0, 1, 0);
            i = func_ov004_020ad674();
            func_ov004_020afdd0(*(int *)(data_ov006_0213ecb8[i] + 0x54), 0x90, xpos, -1, -1);
            func_ov004_020b1ea4(0xa0, xpos, I(0x5964), 0, 0, 1, 0);
            func_ov004_020b1ea4(0xe0, xpos, I(0x5964) * 0x64, 0, 0, 1, 0);
            xpos += 0x18;
            col++;
        }

        /* Loop C: rows 1..4 */
        {
        int t0 = 0x30, t1 = 0x80, t2 = 0x90, t3 = 0xa0, t4 = 0xe0;
        int one = 1, neg = -1;
        limit = col * 0x14;
        for (r7 = 0; r7 < 4; r7++) {
            if (I(0x5960) >= limit && *(int *)(g + r7 * 4 + 0x5968) > 0) {
                int val;
                int *pcnt;
                func_ov004_020afdd0(data_ov006_02138ae0[r7 + 1], t0, xpos, neg, neg);
                val = data_ov006_0212edfc[r7];
                func_ov004_020b1ea4(t1, xpos, val, 0, 0, one, 0);
                i = func_ov004_020ad674();
                func_ov004_020afdd0(*(int *)(data_ov006_0213ecb8[i] + 0x54), t2, xpos, neg, neg);
                pcnt = (int *)(((long long)(int)(g + r7 * 4 + 0x5968)) & 0xFFFFFFFFFFFFFFFFLL);
                func_ov004_020b1ea4(t3, xpos, *pcnt, 0, 0, 0, 0);
                func_ov004_020b1ea4(t4, xpos, val * *pcnt, 0, 0, one, 0);
                xpos += 0x18;
                limit += 0x14;
                col++;
            }
        }
        }

        /* Block D */
        if (I(0x597c) > 0 && I(0x5960) >= col * 0x14) {
            func_ov004_020afdd0(data_ov006_0213765c[0], 0x30, xpos, -1, -1);
            func_ov004_020b1ea4(0x80, xpos, 0x12c, 0, 0, 1, 0);
            i = func_ov004_020ad674();
            func_ov004_020afdd0(*(int *)(data_ov006_0213ecb8[i] + 0x54), 0x90, xpos, -1, -1);
            func_ov004_020b1ea4(0xa0, xpos, I(0x597c), 0, 0, 0, 0);
            func_ov004_020b1ea4(0xe0, xpos, I(0x597c) * 0x12c, 0, 0, 1, 0);
            xpos += 0x18;
            col++;
        }

        /* Block E */
        if (I(0x5984) > 0 && I(0x5960) >= col * 0x14) {
            func_ov004_020afdd0(data_ov006_02137a78[0], 0x30, xpos, -1, -1);
            func_ov004_020b1ea4(0x80, xpos, 0xbb8, 0, 0, 1, 0);
            i = func_ov004_020ad674();
            func_ov004_020afdd0(*(int *)(data_ov006_0213ecb8[i] + 0x54), 0x90, xpos, -1, -1);
            func_ov004_020b1ea4(0xa0, xpos, I(0x5984), 0, 0, 0, 0);
            func_ov004_020b1ea4(0xe0, xpos, I(0x5984) * 0xbb8, 0, 0, 1, 0);
            xpos += 0x18;
            col++;
        }

        /* Block F */
        if (I(0x5980) > 0 && I(0x5960) >= col * 0x14) {
            func_ov004_020afdd0(data_ov006_02137a6c[0], 0x30, xpos, -1, -1);
            func_ov004_020b1ea4(0x80, xpos, 0x7d0, 0, 0, 1, 0);
            i = func_ov004_020ad674();
            func_ov004_020afdd0(*(int *)(data_ov006_0213ecb8[i] + 0x54), 0x90, xpos, -1, -1);
            func_ov004_020b1ea4(0xa0, xpos, I(0x5980), 0, 0, 0, 0);
            func_ov004_020b1ea4(0xe0, xpos, I(0x5980) * 0x7d0, 0, 0, 1, 0);
            xpos += 0x18;
            col++;
        }

        /* Loop G: rows with per-row multiplier */
        limG = col * 0x14;
        for (j = 0; j < 3; j++) {
            int *pcnt2 = (int *)(((long long)(int)(g + j * 4 + 0x5988)) & 0xFFFFFFFFFFFFFFFFLL);
            if (*pcnt2 > 0 && I(0x5960) >= limG) {
                int mult;
                switch (j) {
                case 0:
                    func_ov004_020afdd0(data_ov006_02137a0c[1], 0x20, xpos, -1, -1);
                    func_ov004_020afdd0(data_ov006_02137a0c[1], 0x30, xpos, -1, -1);
                    func_ov004_020afdd0(data_ov006_02137a0c[1], 0x40, xpos, -1, -1);
                    mult = 5000;
                    break;
                case 1:
                    func_ov004_020afdd0(data_ov006_02137a0c[0], 0x20, xpos, -1, -1);
                    func_ov004_020afdd0(data_ov006_02137a0c[0], 0x30, xpos, -1, -1);
                    func_ov004_020afdd0(data_ov006_02137a0c[0], 0x40, xpos, -1, -1);
                    mult = 8000;
                    break;
                case 2:
                    func_ov004_020afdd0(data_ov006_02137a0c[2], 0x20, xpos, -1, -1);
                    func_ov004_020afdd0(data_ov006_02137a0c[2], 0x30, xpos, -1, -1);
                    func_ov004_020afdd0(data_ov006_02137a0c[2], 0x40, xpos, -1, -1);
                    mult = 10000;
                    break;
                }
                func_ov004_020b1ea4(0x80, xpos, mult, 0, 0, 1, 0);
                i = func_ov004_020ad674();
                func_ov004_020afdd0(*(int *)(data_ov006_0213ecb8[i] + 0x54), 0x90, xpos, -1, -1);
                func_ov004_020b1ea4(0xa0, xpos, *pcnt2, 0, 0, 0, 0);
                func_ov004_020b1ea4(0xe0, xpos, mult * *pcnt2, 0, 0, 1, 0);
                xpos += 0x18;
                limG += 0x14;
                col++;
            }
        }

        /* Block H */
        if (I(0x5978) > 0 && I(0x5960) >= col * 0x14) {
            func_ov004_020afdd0(data_ov006_02138ae0[5], 0x30, xpos, -1, -1);
            func_ov004_020b1ea4(0x80, xpos, 0x2710, 0, 0, 1, 0);
            i = func_ov004_020ad674();
            func_ov004_020afdd0(*(int *)(data_ov006_0213ecb8[i] + 0x54), 0x90, xpos, -1, -1);
            func_ov004_020b1ea4(0xa0, xpos, I(0x5978), 0, 0, 0, 0);
            func_ov004_020b1ea4(0xe0, xpos, I(0x5978) * 0x2710, 0, 0, 1, 0);
            xpos += 0x18;
            col++;
        }

        /* Block I */
        if (I(0x5994) > 0 && I(0x5960) >= col * 0x14) {
            func_ov004_020afdd0(data_ov006_021382e0[0], 0x30, xpos, -1, -1);
            func_ov004_020b1ea4(0x80, xpos, 0x7d0, 0, 0, 1, 0);
            i = func_ov004_020ad674();
            func_ov004_020afdd0(*(int *)(data_ov006_0213ecb8[i] + 0x54), 0x90, xpos, -1, -1);
            func_ov004_020b1ea4(0xa0, xpos, I(0x5994), 0, 0, 0, 0);
            func_ov004_020b1ea4(0xe0, xpos, I(0x5994) * 0x7d0, 0, 0, 1, 0);
            xpos += 0x18;
            col++;
        }

        /* Block J: timer */
        if (I(0x5960) >= col * 0x14) {
            if ((I(8) & 0xff) == 0)
                func_ov004_020afdd0(data_ov006_02138bd0[0], 0x40, xpos, -1, -1);
            else
                func_ov004_020afdd0(data_ov006_02138bf4[0], 0x40, xpos, -1, -1);
            func_ov004_020b1ea4(0xe0, xpos, func_ov004_020adbc0(), 0, 0, 1, 0);
            col++;
            if (I(0x5960) < 0x118)
                I(0x5960) = 0x118;
        }

        /* Block K */
        if (I(0x4660) == 1 && I(0x5960) >= 0x12c) {
            if (col >= 7)
                func_ov004_020b0d8c(g, 0x80, 0xa8);
            else if (col == 6)
                func_ov004_020b0d8c(g, 0xe0, 0xa8);
            else
                func_ov004_020b0d8c(g, 0xe0, 0xa0);
        }
    }

    /* merge */
    if ((I(8) & 0xff) != 0 || UC(0x595c) == 0)
        func_ov006_02114b10(g);

    /* Section 2 */
    func_ov004_020b1a5c(func_ov004_020adbc0(), 6);
    func_ov004_020b19f0((void *)func_ov004_020adc1c());

    /* Loop 2: item list (sb reused as index) */
    {
        int sb;
        for (sb = 0; sb < 5; sb++) {
            int *pcnt = LP(g + sb * 4 + 0x478c);
            int cnt = *pcnt;
            if (cnt > 0) {
                int *pmode = LP(g + sb * 4 + 0x47a0);
                int mode = *pmode;
                if (mode >= 2) {
                    unsigned int u;
                    int nd;
                    int *py = LP(g + sb * 8 + 0x47cc);
                    int *px = LP(g + sb * 8 + 0x47c8);
                    func_ov004_020b1ea4((*px >> 12) - 0x10, *py >> 12, cnt, -1, 0, 0, 0);
                    nd = 0;
                    u = (unsigned int)*(int *)(g + sb * 4 + 0x478c);
                    if (u != 0) {
                        do {
                            u /= 10;
                            nd++;
                        } while (u != 0);
                    }
                    i = func_ov004_020ad674();
                    func_ov004_020afdd0(*(int *)(data_ov006_0213ecb8[i] + 0x54),
                                        (*px >> 12) + nd * 8 - 8, *py >> 12, -1, -1);
                    nd = 0;
                    u = (unsigned int)*pcnt;
                    if (u != 0) {
                        do {
                            u /= 10;
                            nd++;
                        } while (u != 0);
                    }
                    func_ov004_020b1ea4((*px >> 12) + nd * 8 + 8, *py >> 12, *pmode, -1, 0, 0, 0);
                } else {
                    int y = *(int *)(g + sb * 8 + 0x47cc);
                    int x = *(int *)(g + sb * 8 + 0x47c8);
                    func_ov004_020b1ea4(x >> 12, y >> 12, cnt, -1, 0, 0, 0);
                }
            }
        }
    }

    /* Loop 3: 16 slots */
    {
        int r4;
        int two = 2, neg = -1, zero = 0;
        for (r4 = 0; r4 < 0x10; r4++) {
            if (*(unsigned char *)(g + r4 + 0x4804) == 1) {
                int idx = *(int *)(g + r4 * 4 + 0x4814) / 5;
                if (idx > 3)
                    idx = two;
                func_ov004_020afdd0(data_ov006_02137a60[idx],
                                    *(int *)(g + r4 * 8 + 0x4854) >> 12,
                                    *(int *)(g + r4 * 8 + 0x4858) >> 12, neg, zero);
            }
        }
    }

    /* Block M */
    {
        int v = I(0x5958);
        int flag = 0;
        if (v > 0) {
            if (v % 30 > 0xf)
                flag = 1;
        } else {
            if (I(0x4784) > 0)
                flag = 1;
        }
        if ((I(8) & 0xff) == 0)
            func_ov004_020afdd0(data_ov006_02138a40[flag], 0x18, -0x90, -1, 0);
        else
            func_ov004_020afdd0(data_ov006_02138a88[flag], 0x18, -0x90, -1, 0);
    }

    /* Block N */
    {
        int v = I(0x5958);
        int flag = 0;
        if (v > 0) {
            if (v % 30 > 0xf)
                flag = 1;
        } else {
            if (I(0x4788) > 0)
                flag = 1;
        }
        if ((I(8) & 0xff) == 0)
            func_ov004_020afdd0(data_ov006_02138a40[flag], 0xe8, -0x90, -1, -1);
        else
            func_ov004_020afdd0(data_ov006_02138a88[flag], 0xe8, -0x90, -1, -1);
    }

    /* Dispatch loops */
    {
        int r4;
        for (r4 = 0; r4 < I(0x4674); r4++)
            ((DispObj *)*(void **)(g + r4 * 4 + 0x4740))->f1();
    }
    if (*(void **)(g + 0x4778) != 0)
        ((DispObj *)*(void **)(g + 0x4778))->f1();
    {
        int r4;
        for (r4 = 0; r4 < I(0x4668); r4++)
            ((DispObj *)*(void **)(g + r4 * 4 + 0x4688))->f1();
    }
    {
        int r4;
        for (r4 = 0; r4 < I(0x466c); r4++)
            ((DispObj *)*(void **)(g + r4 * 4 + 0x4720))->f1();
    }
    {
        int r4;
        for (r4 = 0; r4 < I(0x4670); r4++)
            ((DispObj *)*(void **)(g + r4 * 4 + 0x46bc))->f1();
    }
    {
        int r4;
        for (r4 = 0; r4 < I(0x4678); r4++)
            ((DispObj *)*(void **)(g + r4 * 4 + 0x474c))->f1();
    }
    {
        int r4;
        for (r4 = 0; r4 < I(0x467c); r4++)
            ((DispObj *)*(void **)(g + r4 * 4 + 0x4764))->f1();
    }
    {
        int r4;
        for (r4 = 0; r4 < I(0x4680); r4++)
            ((DispObj *)*(void **)(g + r4 * 4 + 0x4770))->f1();
    }
    ((DispObj *)*(void **)(g + 0x4684))->f1();
    if (*(void **)(g + 0x477c) != 0)
        ((DispObj *)*(void **)(g + 0x477c))->f1();
    if (*(void **)(g + 0x4780) != 0)
        ((DispObj *)*(void **)(g + 0x4780))->f1();
    {
        int r5;
        for (r5 = 0; r5 < I(0x4668); r5++)
            func_ov006_02113c14(*(void **)(g + r5 * 4 + 0x4688));
    }

    if ((I(8) & 0xff) == 0 && UC(0x595c) != 0)
        func_ov006_02114b10(g);

    return 1;
}
