//cpp
#include "nitro/fx/fx.h"
#include "Ov007MenuState.h"
#include "cstd.h"

extern "C" {

extern short data_02082214[];

/* The 10-byte layout row an item points at (names as in
 * func_ov007_020ae2d0); only the home row is read here. */
typedef struct Ov007ItemLayout {
    u16 kind;  /* 0x0 */
    s16 homeX; /* 0x2 */
    s16 homeY; /* 0x4 */
} Ov007ItemLayout;

/* The 0x2c-byte record func_ov007_020aea6c allocates; names as in
 * func_ov007_020ae2d0, only the slots this file touches. */
typedef struct Ov007Item {
    u16 id; /* 0x00: func_ov007_020b4c04's index */
    char pad_02[2];
    Ov007MenuState *state; /* 0x04: func_ov007_020c9388(1) */
    s32 posX;              /* 0x08: Q12 */
    s32 posY;              /* 0x0c: Q12 */
    char pad_10[8];
    s32 wiggleTimer;         /* 0x18: func_ov007_020ae834 counts it down */
    s32 wiggleFrames;        /* 0x1c: the wiggle's length */
    s32 ready;               /* 0x20 */
    Ov007ItemLayout *layout; /* 0x24 */
} Ov007Item;

typedef char Ov007Item_layout_must_be_at_0x24[(int)&((Ov007Item *)0)->layout == 0x24 ? 1 : -1];

/* The 0x1c record func_ov007_020b4c04 returns; only its item here. */
typedef struct Ov007ItemView {
    Ov007Item *item; /* 0x00 */
} Ov007ItemView;

/* Item 1's wiggle: while its state is idle and wiggleTimer runs, posY swings
 * about the layout's home row on a sine that fades out with the remaining
 * time. The parameter keeps the tree's char * spelling; `s` is the typed
 * item. */
void func_ov007_020ae454(char *c) {
    Ov007Item *s = ((Ov007ItemView *)c)->item;
    if (s->id != 1)
        return;
    if (s->state->currentState != 0)
        return;
    int a = s->wiggleTimer;
    if (a > 0) {
        int b = s->wiggleFrames;
        int d = b - a;
        int r4;
        if (d <= 0)
            r4 = 0;
        else if (d >= b)
            r4 = 0x1000;
        else
            r4 = cstd::div(d << 0xc, b);
        int t = ((r4 * (s->wiggleFrames * 0x2d)) >> 0xc) << 0x10;
        int r = cstd::div(t, 0x168);
        r = (unsigned short)r;
        int sv = data_02082214[(r >> 4) << 1];
        int inv = 0x1000 - r4;
        int prod = FX_MUL(sv, inv);
        int e = s->layout->homeY;
        s->posY = (e << 0xc) + (prod << 2);
    } else {
        int e = s->layout->homeY;
        s->posY = e << 0xc;
    }
}
}
