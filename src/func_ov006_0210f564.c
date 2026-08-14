#include "types.h"

/* cMgSmartball_board_c's slot 0 (SaveSnapshot). The class is not migrated yet,
 * so this is written against raw offsets. Like every sibling's slot 0 it opens
 * with the base's own SaveSnapshot body written out inline rather than called.
 *
 * Then: spend the countdown at +0x94 and, once a second, replay the jingle for
 * the line count at +0x98; age the eight per-line timers at +0x6c; scan the
 * shared manager's up-to-13 tracked balls for one sitting in the board's strip,
 * map it to one of the nine cells and claim the cell; finally spin every
 * claimed cell's angle at +0x3c up to its 0x8000 stop. */

typedef struct { int x; int y; } V2;

typedef struct {
    int f0;          /* 0x00 */
    char *f4;        /* 0x04 manager */
    int f8;          /* 0x08 x */
    int fc;          /* 0x0c y */
    int f10;         /* 0x10 snapshot x */
    int f14;         /* 0x14 snapshot y */
    u8 pad18[0x19];  /* 0x18 */
    u8 cells[9];     /* 0x31 */
    u8 pad3a[2];     /* 0x3a */
    int angles[9];   /* 0x3c */
    u8 f60[12];      /* 0x60 */
    int lines[8];    /* 0x6c */
    u8 marks[8];     /* 0x8c */
    int f94;         /* 0x94 */
    int f98;         /* 0x98 */
} S;

extern int data_ov006_02142c1c[];

extern void func_ov006_0211470c(int *a, int *b);
extern int func_ov006_02111dcc(char *p, int val);
extern void func_ov006_02114800(void *c, int *p, int f);
extern void func_ov006_0210ef48(void *c, int i);
extern void func_02012718(void *a, int b);
extern void _ZN5Sound12PlayBank2_2DEj(u32 id);

static inline u8 *GetObj(char *g, int i)
{
    return i >= 13 ? 0 : ((u8 **)(g + 0x4688))[i];
}

static inline void SetV2(V2 *p, int x, int y)
{
    p->x = x;
    p->y = y;
}

#pragma opt_strength_reduction off
void func_ov006_0210f564(S *c)
{
    int i;
    int n;
    int j;
    int lo;
    int hi;
    V2 v;
    V2 t;
    V2 a;
    V2 b;
    V2 d;

    c->f10 = c->f8;
    c->f14 = c->fc;
    if (c->f94 > 0) {
        (*(int *)((int)c + 0x94))--;
        if (c->f94 > 0 && c->f94 % 60 == 0) {
            switch (c->f98) {
            case 0:
            case 1:
                _ZN5Sound12PlayBank2_2DEj(0x176);
                break;
            case 2:
                _ZN5Sound12PlayBank2_2DEj(0x177);
                break;
            case 3:
                _ZN5Sound12PlayBank2_2DEj(0x178);
                break;
            case 4:
            default:
                _ZN5Sound12PlayBank2_2DEj(0x179);
                break;
            }
        }
    }
    for (i = 0; i < 8; i++) {
        if (c->lines[i] > 0)
            (*(int *)((int)c + i * 4 + 0x6c))--;
    }
    for (n = 0; n < *(int *)(c->f4 + 0x4668); n++) {
        if (*(u8 *)(c->f4 + 0x595c) != 0)
            break;
        if (GetObj(c->f4, n)[0x30] == 0)
            continue;
        func_ov006_0211470c((int *)&t, (int *)GetObj(c->f4, n));
        v = t;
        if (v.y < 0x74000)
            continue;
        if (v.y >= 0x7c000)
            continue;
        for (j = 0, lo = 0xc000, hi = 0x14000; j < 9; j++) {
            int slot = data_ov006_02142c1c[j];
            if (v.x >= lo && v.x < hi) {
                if (func_ov006_02111dcc((char *)GetObj(c->f4, n), 0x14)) {
                    if (c->cells[slot] == 0) {
                        SetV2(&a, c->f8 + ((data_ov006_02142c1c[j] % 3 - 1) * 0x18 << 12),
                              c->fc + ((data_ov006_02142c1c[j] / 3 - 1) * 0x18 << 12));
                        func_ov006_02114800(c->f4, (int *)&a, 0);
                        SetV2(&b, (j * 0x18 + 0x10) << 12, 0x78000);
                        func_ov006_02114800(c->f4, (int *)&b, 0);
                        func_02012718((void *)0x1be, j * 0x18000 + 0x10000);
                    } else {
                        SetV2(&d, (j * 0x18 + 0x10) << 12, 0x78000);
                        func_ov006_02114800(c->f4, (int *)&d, 1);
                        func_02012718((void *)0x17a, j * 0x18000 + 0x10000);
                    }
                    func_ov006_0210ef48(c, j);
                    break;
                }
            }
            lo += 0x18000;
            hi += 0x18000;
        }
    }
    for (i = 0; i < 9; i++) {
        if (c->cells[i] == 1) {
            (*(int *)((int)c + i * 4 + 0x3c)) += 0x400;
            if (c->angles[i] >= 0x8000)
                c->angles[i] = 0x8000;
        }
    }
}
