//cpp
#include "types.h"
#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "Ov007PaletteBuf.h"
#include "nitro/fx/fx_cp.h"
#include "math/ProgressRatio.h"

/* 32-colour palettes, 0x40 bytes each */
extern char data_ov007_02102f28[];

extern "C" {
extern void func_ov007_020c92d0(Ov007MenuState *state);
extern void func_ov007_020c940c(Ov007PaletteBuf *buf);
extern void func_ov007_020c93c4(int t, Ov007PaletteBuf *from, Ov007PaletteBuf *to,
                                Ov007PaletteBuf *out);
}

/* Fades the menu palette to palette n over `frames` frames: on entry it
 * loads n into buffer 2, and snapshots the shown colours (buffer 0) into
 * buffer 1, or with frames 0 copies n straight into buffer 0. While the
 * fade runs buffer 0 is blended from 1 to 2 and the state's stateWork is
 * 1. */
extern "C" void func_ov007_020bce70(short n, int frames) {
    int t;

    sMenuWork->paletteState->requestedState = n;
    func_ov007_020c92d0(sMenuWork->paletteState);

    t = sMenuWork->paletteState->stateTicks;
    if (t == 0) {
        sMenuWork->paletteBufs[2]->source = data_ov007_02102f28 + (n << 6);
        func_ov007_020c940c(sMenuWork->paletteBufs[2]);
        if (frames == 0) {
            t = frames + 1;
            func_ov007_020c940c(sMenuWork->paletteBufs[0]);
        } else {
            func_ov007_020c940c(sMenuWork->paletteBufs[1]);
        }
    }

    if (t >= 0 && t <= frames) {
        int ratio;
        ratio = ProgressRatio(t, frames);
        func_ov007_020c93c4(ratio, sMenuWork->paletteBufs[1], sMenuWork->paletteBufs[2],
                            sMenuWork->paletteBufs[0]);
        sMenuWork->paletteState->stateWork = 1;
    } else {
        if (sMenuWork->paletteState->stateWork != 0)
            sMenuWork->paletteState->stateWork = 0;
    }
}
