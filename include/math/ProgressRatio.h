#ifndef SM64DS_MATH_PROGRESSRATIO_H
#define SM64DS_MATH_PROGRESSRATIO_H

#include "nitro/fx/fx.h"

/* How far a timed step has run: ticks over frames as a Q12 fraction, held
 * to [0, 1]. The file-select overlay (ov007) drives its slides, fades and
 * bounces from this; func_ov007_020aeaec is the out-of-line form for an
 * item's state ticks.
 *
 * Only the files that call it include this header. A function body in a
 * header takes compiler @NNN numbers in every file that includes it. */
static inline int ProgressRatio(int ticks, int frames) {
    if (ticks <= 0)
        return 0;
    if (ticks >= frames)
        return 0x1000;
    return FX_DivS32(ticks << 12, frames);
}

/* The same for a step that starts late: how far v has got from lo to hi. */
static inline int RangeRatio(int v, int lo, int hi) {
    if (v <= lo)
        return 0;
    if (v >= hi)
        return 0x1000;
    return FX_DivS32((v - lo) << 12, hi - lo);
}

#endif
