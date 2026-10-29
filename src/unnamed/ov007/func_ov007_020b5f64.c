#include "types.h"
#include "Ov007MenuState.h"
#include "Ov007Tween.h"
#include "Ov007Work.h"
extern u8 data_ov007_0210342c[];

/* The 10-byte layout row an item points at (names as in
 * func_ov007_020b5bf0); only the home position is read here. */
typedef struct Ov007ItemLayout {
    u16 kind;  /* 0x0 */
    s16 homeX; /* 0x2 */
    s16 homeY; /* 0x4 */
} Ov007ItemLayout;

/* The 0x2c-byte record func_ov007_020aea6c allocates; names as in
 * func_ov007_020b5bf0 and func_ov007_020beeb0, only the slots this file
 * touches. */
typedef struct Ov007Item {
    u16 id; /* 0x00: func_ov007_020b4c04's index */
    char pad_02[2];
    Ov007MenuState *state; /* 0x04: func_ov007_020c9388(1) */
    s32 posX;              /* 0x08: Q12 */
    s32 posY;              /* 0x0c */
    char pad_10[0x14];
    Ov007ItemLayout *layout; /* 0x24 */
} Ov007Item;

/* The 0x1c record func_ov007_020b4c04 returns (names as in that file): this
 * function slides item from slideFrom to slideTo by slideT. */
typedef struct Ov007ItemView {
    Ov007Item *item; /* 0x00 */
    char pad_04[8];
    s32 slideT;             /* 0x0c: slideTween output, 0..0x1000 */
    Ov007Tween *slideTween; /* 0x10: func_ov007_020c908c, bound to slideT */
    s16 slideFromX;         /* 0x14 */
    s16 slideFromY;         /* 0x16 */
    s16 slideToX;           /* 0x18 */
    s16 slideToY;           /* 0x1a */
} Ov007ItemView;

/* The extern keeps this file's u8[] spelling; the manager pointer is stored there. */
#define WORK (*(Ov007Work **)data_ov007_0210342c)

/* The parameter keeps the tree's char * spelling (the declaration-agreement
 * gate); the typed view is a local. */
void func_ov007_020b5f64(char *self)
{
    Ov007ItemView *view = (Ov007ItemView *)self;
    Ov007Item *p;
    Ov007Work *g;
    int slot;
    int x;
    int sel;
    int y;
    Ov007Tween *q;
    int w;

    p = view->item;
    slot = p->id - 0xe;
    g = WORK;
    sel = g->states[1]->currentState;
    x = view->slideToX;
    y = view->slideToY;

    switch (sel) {
    case 0:
        x = p->layout->homeX;
        y = p->layout->homeY;
        break;
    case 1:
        y = 0x24;
        break;
    case 3:
    case 6:
        y = 0x58;
        break;
    case 7:
        if (slot == g->selectedSlot || slot == g->targetSlot)
            y = 0x24;
        else
            y = 0x58;
        break;
    case 9:
        if (slot == g->selectedSlot)
            y = 0x24;
        else
            y = 0x58;
        break;
    }

    if (p->state->stateWork == 2)
        p->state->stateWork = 0;

    if (x != view->slideToX || y != view->slideToY) {
        view->slideFromX = (short)(p->posX >> 12);
        view->slideFromY = (short)(p->posY >> 12);
        view->slideToX = x;
        view->slideToY = y;
        q = view->slideTween;
        q->state = 1;
        q->mode = 1;
        q->t = 0;
        q->step = 0xa0;
        q->from = 0;
        *q->out = q->from;
        q->to = 0x1000;
    } else if (view->slideTween->state == 3) {
        if (p->state->stateWork == 1)
            p->state->stateWork = 2;
    }

    if (view->slideTween->state == 3)
        return;

    w = view->slideT;
    p->posX = (int)(((s64)(0x1000 - w) * ((int)view->slideFromX << 12) +
                     (s64)w * ((int)view->slideToX << 12)) >>
                    12);
    p->posY = (int)(((s64)(0x1000 - w) * ((int)view->slideFromY << 12) +
                     (s64)w * ((int)view->slideToY << 12)) >>
                    12);
}
