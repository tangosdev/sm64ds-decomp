//cpp
#include "types.h"
#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "cstd.h"

extern "C" {

extern u8 data_ov007_0210342c[];
extern short data_02082214[];

/* The 10-byte layout row an item points at (names as in
 * func_ov007_020ae834); only the home row is read here. */
typedef struct Ov007ItemLayout {
    u16 kind;  /* 0x0 */
    s16 homeX; /* 0x2 */
    s16 homeY; /* 0x4 */
} Ov007ItemLayout;

/* The 0x2c-byte record func_ov007_020aea6c allocates; names as in
 * func_ov007_020b5bf0, func_ov007_020b5f64 and func_ov007_020beeb0, only the
 * slots this file touches. */
typedef struct Ov007Item {
    u16 id; /* 0x00: func_ov007_020b4c04's index; switched on below */
    char pad_02[2];
    Ov007MenuState *state; /* 0x04: func_ov007_020c9388(1) */
    s32 posX;              /* 0x08: Q12 */
    s32 posY;              /* 0x0c */
    char pad_10[8];
    s32 wiggleTimer;         /* 0x18: func_ov007_020ae834 counts it down */
    s32 wiggleFrames;        /* 0x1c: the wiggle's length */
    s32 ready;               /* 0x20: medium confidence (func_ov007_020beeb0) */
    Ov007ItemLayout *layout; /* 0x24 */
} Ov007Item;

/* The 0x1c record func_ov007_020b4c04 returns; only its item here. */
typedef struct Ov007ItemView {
    Ov007Item *item; /* 0x00 */
} Ov007ItemView;

typedef char
    Ov007Work_itemViews_must_be_at_0x114[(int)&((Ov007Work *)0)->itemViews == 0x114 ? 1 : -1];
typedef char Ov007Item_layout_must_be_at_0x24[(int)&((Ov007Item *)0)->layout == 0x24 ? 1 : -1];

/* The extern keeps this file's u8[] spelling; the manager pointer is stored there. */
#define WORK (*(Ov007Work **)data_ov007_0210342c)

/* Per-item idle effects, by item id: item 0 bobs its y about the layout's
 * home row on a 50-frame sine; item 1 arms a 60-frame wiggle every 270 frames
 * once the menu is idle; item 7 is ready only while item view 8's state is 0.
 * The parameter keeps the tree's char * spelling; `view` is the typed local. */
void func_ov007_020ae2d0(char *c)
{
    Ov007ItemView *view = (Ov007ItemView *)c;
    Ov007Item *p = view->item;
    u16 id = p->id;
    int v = p->state->stateTicks;

    switch (id) {
    case 1: {
        Ov007Work *q = WORK;
        short a = q->states[0]->currentState;
        if (a == 0) {
            short b = q->states[1]->currentState;
            if (b == 0) return;
        }
        if (a == 2) return;
        if (v < 0x28) return;
        if (cstd::mod(v - 0x28, 0x10e) != 0)
            return;
        {
            Ov007Item *p2 = view->item;
            p2->wiggleFrames = 0x3c;
            p2->wiggleTimer = p2->wiggleFrames;
        }
        return;
    }
    case 0: {
        int n = cstd::mod(v, 0x32);
        int m, arrIdx;
        short idx, base;
        Ov007Item *q;
        if (n <= 0) n = 0;
        else if (n >= 0x32) n = 0x1000;
        else
            n = cstd::div(n << 0xc, 0x32);
        m = (-n) * 0xffff;
        m = (int)(((unsigned)(m << 4)) >> 0x10);
        arrIdx = m >> 4;
        idx = data_02082214[arrIdx * 2];
        q = view->item;
        base = q->layout->homeY;
        q->posY = idx * 3 + (base << 12);
        return;
    }
    case 7: {
        Ov007Work *q = WORK;
        Ov007ItemView *r = q->itemViews[8];
        Ov007Item *s = r->item;
        short t = s->state->currentState;
        if (t == 0) {
            if (p->ready == 0)
                p->ready = 1;
        } else {
            if (p->ready != 0)
                p->ready = 0;
        }
        return;
    }
    }
}
}
