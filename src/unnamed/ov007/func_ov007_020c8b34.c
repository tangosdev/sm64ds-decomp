#include "Ov007Transition.h"

typedef long long s64;
/* src/engine/gx/G2x.cpp */
extern void G2x_ChangeBlendBrightness_(volatile unsigned short *reg, int v);
inline int inline_fn(int arg0) {
    return arg0;
}

/* Eases the master brightness (effect 4) from fx.brightness.from to
 * fx.brightness.to by t (Q12) on the transition's screens. */
void func_ov007_020c8b34(Ov007Transition *tr, int t) {
    int new_var2;
    int v = tr->fx.brightness.to;
    int ip = tr->fx.brightness.from;
    int new_var;
    if (t < 0x1000) {
        if (t <= 0) {
            v = ip;
        } else {
            int mid =
                (int)(((((s64)t) * (inline_fn(v) << 0xc)) + (((s64)(0x1000 - t)) * (ip << 0xc))) >>
                      0xc);
            new_var = mid;
            v = new_var >> 0xc;
        }
    }
    if (tr->screens & 1) {
        new_var2 = 0x4000050;
        G2x_ChangeBlendBrightness_((volatile unsigned short *)new_var2, v);
    }
    if (tr->screens & 2) {
        G2x_ChangeBlendBrightness_((volatile unsigned short *)0x4001050, v);
    }
}
