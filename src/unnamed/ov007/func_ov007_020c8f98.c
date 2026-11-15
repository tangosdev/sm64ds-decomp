#include "Ov007Tween.h"

extern void func_ov007_020c90c0(void *c);

/* One frame of a scalar tween (modes as in Ov007Tween.h). Returns 1 once it
 * has arrived (state 3). */
int func_ov007_020c8f98(Ov007Tween *tw) {
    s16 t;
    int a;
    int b;
    s32 *slot;

    if (tw->state == 2) {
        tw->state = 3;
    } else if (tw->state == 1) {
        switch (tw->mode) {
        case 0:
            t = tw->t;
            a = tw->to;
            b = tw->from;
            if (t < 0x1000) {
                if (t <= 0)
                    a = b;
                else
                    a = (int)(((s64)(0x1000 - t) * b + (s64)t * a) >> 12);
            }
            slot = tw->out;
            *slot = a;
            func_ov007_020c90c0(tw);
            break;
        case 1: {
            int c;
            int target;
            int st;
            int d;
            int ad;
            int sg;
            int dn; /* stays a flag: setting tw->state inside the arm shrinks it (0xdc) */

            c = *tw->out;
            target = tw->to;
            st = tw->step;
            d = target - c;
            if (d >= 0)
                ad = d;
            else
                ad = -d;
            if (ad <= st) {
                *tw->out = target;
                dn = 1;
            } else {
                if (d >= 0)
                    sg = 1;
                else
                    sg = -1;
                *tw->out = st * sg + c;
                dn = 0;
            }
            if (dn != 0)
                tw->state = 2;
            break;
        }
        default:
            break;
        }
    }

    if (tw->state == 3)
        return 1;
    return 0;
}
