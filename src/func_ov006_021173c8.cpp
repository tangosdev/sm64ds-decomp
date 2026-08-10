//cpp
// MATCH: func_ov006_021173c8 @ 0x021173c8 size 0x10c0 (ov006), mwccarm 2004/b56.
// Levers from near-miss div=15:
//  - plain (non-u64) pcnt for loop C → pcnt@sp+0x10 (div 15→8)
//  - block-scope pcnt2 for loop G → val@sp+0x14 (div 8→5)
//  - int *slot[2] with early slot[0]=0, slot[1]=loopG pcnt, slot[0]=loop2 pmode
//    forces spill order pmode@0x18 before pcnt2@0x1c (div 5→0)
// Loop2 digit-count still uses inner u64 mask form for pooled ldr form.
//
// TODO: shared ov006 HUD/score object layout — this file and siblings
// (func_ov006_020fd2d8, 02103ac0, 020fb7e0) re-spell the same base offsets:
//   0x4660–0x4770  display-object pointer arrays (DispObj* at k*4 stride)
//   0x5958–0x5998  score/coin block
// Name the base struct once so later matches do not invent a fifth spelling.

/* Both pragmas are load-bearing, not tidying: without them mwccarm 2004/b56
 * emits 0x1170 bytes with 999 words differing (target is 0x10c0). Do not remove. */
#pragma opt_common_subs off
#pragma opt_strength_reduction off

#include "private/disp_obj_vtbl.h"

extern "C" {
void SetSubBg0Offset(void *p, int a);
void func_ov004_020afdd0(int a0, int a1, int a2, int a3, int a4);
void func_ov004_020b1ea4(int a0, int a1, int a2, int a3, int a4, int a5, int a6);
int GetGameLanguage(void);
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
#define LP(e) ((int *)(e))

extern "C" int func_ov006_021173c8(void *this_)
{
    char *g = (char *)this_;
    int col;
    int xpos;
    int row;
    int limit;
    int i;
    int j;
    int limG;
    int *pcnt;
    int val;
    int *slot[2];

    SetSubBg0Offset(*(void **)(g + 0x5998), 0);
    col = 0;

    if (I(0x4660) == 1 && I(0x5998) <= 0) {
        xpos = 0x18;
        slot[0] = (int *)0;

        /* Block B: row 0 */
        if (I(0x5964) > 0) {
            func_ov004_020afdd0(data_ov006_02138ae0[0], 0x30, xpos, -1, -1);
            func_ov004_020b1ea4(0x80, xpos, 0x64, 0, 0, 1, 0);
            i = GetGameLanguage();
            func_ov004_020afdd0(*(int *)(data_ov006_0213ecb8[i] + 0x54), 0x90, xpos, -1, -1);
            func_ov004_020b1ea4(0xa0, xpos, I(0x5964), 0, 0, 1, 0);
            func_ov004_020b1ea4(0xe0, xpos, I(0x5964) * 0x64, 0, 0, 1, 0);
            xpos += 0x18;
            col++;
        }

        /* Loop C: rows 1..4 */
        limit = col * 0x14;
        for (row = 0; row < 4; row++) {
            if (I(0x5960) >= limit && *(int *)(g + row * 4 + 0x5968) > 0) {
                func_ov004_020afdd0(data_ov006_02138ae0[row + 1], 0x30, xpos, -1, -1);
                val = data_ov006_0212edfc[row];
                func_ov004_020b1ea4(0x80, xpos, val, 0, 0, 1, 0);
                i = GetGameLanguage();
                func_ov004_020afdd0(*(int *)(data_ov006_0213ecb8[i] + 0x54), 0x90, xpos, -1, -1);
                pcnt = (int *)(g + row * 4 + 0x5968);
                func_ov004_020b1ea4(0xa0, xpos, *pcnt, 0, 0, 0, 0);
                func_ov004_020b1ea4(0xe0, xpos, val * *pcnt, 0, 0, 1, 0);
                xpos += 0x18;
                limit += 0x14;
                col++;
            }
        }

        /* Block D */
        if (I(0x597c) > 0 && I(0x5960) >= col * 0x14) {
            func_ov004_020afdd0(data_ov006_0213765c[0], 0x30, xpos, -1, -1);
            func_ov004_020b1ea4(0x80, xpos, 0x12c, 0, 0, 1, 0);
            i = GetGameLanguage();
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
            i = GetGameLanguage();
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
            i = GetGameLanguage();
            func_ov004_020afdd0(*(int *)(data_ov006_0213ecb8[i] + 0x54), 0x90, xpos, -1, -1);
            func_ov004_020b1ea4(0xa0, xpos, I(0x5980), 0, 0, 0, 0);
            func_ov004_020b1ea4(0xe0, xpos, I(0x5980) * 0x7d0, 0, 0, 1, 0);
            xpos += 0x18;
            col++;
        }

        /* Loop G: rows with per-row multiplier */
        limG = col * 0x14;
        for (j = 0; j < 3; j++) {
            slot[1] = (int *)(g + j * 4 + 0x5988);
            if (*slot[1] > 0 && I(0x5960) >= limG) {
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
                i = GetGameLanguage();
                func_ov004_020afdd0(*(int *)(data_ov006_0213ecb8[i] + 0x54), 0x90, xpos, -1, -1);
                func_ov004_020b1ea4(0xa0, xpos, *slot[1], 0, 0, 0, 0);
                func_ov004_020b1ea4(0xe0, xpos, mult * *slot[1], 0, 0, 1, 0);
                xpos += 0x18;
                limG += 0x14;
                col++;
            }
        }

        /* Block H */
        if (I(0x5978) > 0 && I(0x5960) >= col * 0x14) {
            func_ov004_020afdd0(data_ov006_02138ae0[5], 0x30, xpos, -1, -1);
            func_ov004_020b1ea4(0x80, xpos, 0x2710, 0, 0, 1, 0);
            i = GetGameLanguage();
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
            i = GetGameLanguage();
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
                slot[0] = LP(g + sb * 4 + 0x47a0);
                int mode = *slot[0];
                if (mode >= 2) {
                    unsigned int u;
                    int nd;
                    int *py = LP(g + sb * 8 + 0x47cc);
                    int *px = LP(g + sb * 8 + 0x47c8);
                    func_ov004_020b1ea4((*px >> 12) - 0x10, *py >> 12, cnt, -1, 0, 0, 0);
                    nd = 0;
                    u = (unsigned int)*(int *)((int)g + sb * 4 + 0x478c);
                    if (u != 0) {
                        do {
                            u /= 10;
                            nd++;
                        } while (u != 0);
                    }
                    i = GetGameLanguage();
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
                    func_ov004_020b1ea4((*px >> 12) + nd * 8 + 8, *py >> 12, *slot[0], -1, 0, 0, 0);
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
        int slot;
        int two = 2, neg = -1, zero = 0;
        for (slot = 0; slot < 0x10; slot++) {
            if (*(unsigned char *)(g + slot + 0x4804) == 1) {
                int idx = *(int *)(g + slot * 4 + 0x4814) / 5;
                if (idx > 3)
                    idx = two;
                func_ov004_020afdd0(data_ov006_02137a60[idx],
                                    *(int *)(g + slot * 8 + 0x4854) >> 12,
                                    *(int *)(g + slot * 8 + 0x4858) >> 12, neg, zero);
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
        int k;
        for (k = 0; k < I(0x4674); k++)
            ((DispObj *)*(void **)(g + k * 4 + 0x4740))->f1();
    }
    if (*(void **)(g + 0x4778) != 0)
        ((DispObj *)*(void **)(g + 0x4778))->f1();
    {
        int k;
        for (k = 0; k < I(0x4668); k++)
            ((DispObj *)*(void **)(g + k * 4 + 0x4688))->f1();
    }
    {
        int k;
        for (k = 0; k < I(0x466c); k++)
            ((DispObj *)*(void **)(g + k * 4 + 0x4720))->f1();
    }
    {
        int k;
        for (k = 0; k < I(0x4670); k++)
            ((DispObj *)*(void **)(g + k * 4 + 0x46bc))->f1();
    }
    {
        int k;
        for (k = 0; k < I(0x4678); k++)
            ((DispObj *)*(void **)(g + k * 4 + 0x474c))->f1();
    }
    {
        int k;
        for (k = 0; k < I(0x467c); k++)
            ((DispObj *)*(void **)(g + k * 4 + 0x4764))->f1();
    }
    {
        int k;
        for (k = 0; k < I(0x4680); k++)
            ((DispObj *)*(void **)(g + k * 4 + 0x4770))->f1();
    }
    ((DispObj *)*(void **)(g + 0x4684))->f1();
    if (*(void **)(g + 0x477c) != 0)
        ((DispObj *)*(void **)(g + 0x477c))->f1();
    if (*(void **)(g + 0x4780) != 0)
        ((DispObj *)*(void **)(g + 0x4780))->f1();
    {
        int k;
        for (k = 0; k < I(0x4668); k++)
            func_ov006_02113c14(*(void **)(g + k * 4 + 0x4688));
    }

    if ((I(8) & 0xff) == 0 && UC(0x595c) != 0)
        func_ov006_02114b10(g);

    return 1;
}
