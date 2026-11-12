#include "Ov007Transition.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/gx_bgcnt.h"

/* Stops the transition: idle, at t = 1.0, with the request cleared, and
 * turns its effect off on each of its screens. Mosaic (1) clears the mosaic
 * bit of BG0..3CNT; the blend effects (2..4) clear BLDCNT. */
void func_ov007_020c897c(Ov007Transition *tr) {
    tr->state = 0;
    tr->t = 0x1000;
    tr->request = 0;
    switch (tr->effect) {
    case 1:
        if (tr->screens & 1) {
            G2_BG0Mosaic(0);
            G2_BG1Mosaic(0);
            G2_BG2Mosaic(0);
            G2_BG3Mosaic(0);
        }
        if (tr->screens & 2) {
            G2S_BG0Mosaic(0);
            G2S_BG1Mosaic(0);
            G2S_BG2Mosaic(0);
            G2S_BG3Mosaic(0);
        }
        break;
    case 2:
    case 3:
    case 4:
        if (tr->screens & 1) {
            G2_BlendNone();
        }
        if (tr->screens & 2) {
            G2S_BlendNone();
        }
        break;
    }
}
