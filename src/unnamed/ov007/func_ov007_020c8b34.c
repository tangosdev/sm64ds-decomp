#include "Ov007Transition.h"
#include "nitro/gx/g2.h"

/* Eases the blend brightness (effect 4, BLDY) from fx.brightness.from to
 * fx.brightness.to by t (Q12) on the transition's screens. */
void func_ov007_020c8b34(Ov007Transition *tr, int t) {
    int brightness = tr->fx.brightness.to;
    int from = tr->fx.brightness.from;

    if (t < 0x1000) {
        if (t <= 0) {
            brightness = from;
        } else {
            brightness =
                (int)(((s64)(0x1000 - t) * (from << 12) + (s64)t * (brightness << 12)) >> 12) >> 12;
        }
    }
    if (tr->screens & 1)
        G2_ChangeBlendBrightness(brightness);
    if (tr->screens & 2)
        G2S_ChangeBlendBrightness(brightness);
}
