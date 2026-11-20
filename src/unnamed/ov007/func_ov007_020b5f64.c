#include "types.h"
#include "Ov007MenuState.h"
#include "Ov007Tween.h"
#include "Ov007Work.h"
#include "Ov007Item.h"

void func_ov007_020b5f64(Ov007ItemView *view) {
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
    g = gMenuWork;
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
