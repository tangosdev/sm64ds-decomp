//cpp
#include "types.h"
#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "Ov007Item.h"
#include "nitro/fx/fx_trig.h"
#include "nitro/fx/fx_cp.h"
#include "math/ProgressRatio.h"

extern "C" {

typedef char
    Ov007Work_itemViews_must_be_at_0x114[(intptr_t)(&((Ov007Work *)0)->itemViews) == 0x114 ? 1 : -1];

/* Per-item idle effects, by item id: item 0 bobs its y about the layout's
 * home row on a 50-frame sine; item 1 arms a 60-frame wiggle every 270 frames
 * once the menu is idle; item 7 is visible only while item view 8's state
 * is 0. */
void func_ov007_020ae2d0(Ov007ItemNode *self) {
    Ov007Item *p = self->item;
    u16 id = p->id;
    int v = p->state->stateTicks;

    switch (id) {
    case 1: {
        Ov007Work *q = WORK;
        short a = q->states[0]->currentState;
        if (a == 0) {
            short b = q->states[1]->currentState;
            if (b == 0)
                return;
        }
        if (a == 2)
            return;
        if (v < 0x28)
            return;
        if (FX_ModS32(v - 0x28, 0x10e) != 0)
            return;
        {
            Ov007Item *p2 = self->item;
            p2->wiggleFrames = 0x3c;
            p2->wiggleTimer = p2->wiggleFrames;
        }
        return;
    }
    case 0: {
        int n = FX_ModS32(v, 0x32);
        int m;
        short sine, base;
        Ov007Item *q;
        n = ProgressRatio(n, 0x32);
        m = (-n) * 0xffff;
        m = (int)(((unsigned)(m << 4)) >> 0x10);
        sine = FX_SinIdx(m >> 4);
        q = self->item;
        base = q->layout->homeY;
        q->posY = sine * 3 + (base << 12);
        return;
    }
    case 7: {
        Ov007Work *q = WORK;
        Ov007ItemView *r = q->itemViews[8];
        Ov007Item *s = r->item;
        short t = s->state->currentState;
        if (t == 0) {
            if (p->visible == 0)
                p->visible = 1;
        } else {
            if (p->visible != 0)
                p->visible = 0;
        }
        return;
    }
    }
}
}
