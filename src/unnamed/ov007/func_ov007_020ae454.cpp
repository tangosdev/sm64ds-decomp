//cpp
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "Ov007MenuState.h"
#include "Ov007Item.h"
#include "nitro/fx/fx_cp.h"
#include "math/ProgressRatio.h"

extern "C" {

/* Item 1's wiggle: while its state is idle and wiggleTimer runs, posY swings
 * about the layout's home row on a sine that fades out with the remaining
 * time. */
void func_ov007_020ae454(Ov007ItemNode *self) {
    Ov007Item *s = self->item;
    if (s->id != 1)
        return;
    if (s->state->currentState != 0)
        return;
    int a = s->wiggleTimer;
    if (a > 0) {
        int b = s->wiggleFrames;
        int d = b - a;
        int r4;
        r4 = ProgressRatio(d, b);
        int t = ((r4 * (s->wiggleFrames * 0x2d)) >> 0xc) << 0x10;
        int r = FX_DivS32(t, 0x168);
        r = (unsigned short)r;
        int sv = FX_SinIdx(r >> 4);
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
