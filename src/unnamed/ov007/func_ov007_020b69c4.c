#include "types.h"
#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "Ov007PaletteBuf.h"

extern void *func_ov007_020c9388(int initialState);
extern Ov007PaletteBuf *func_ov007_020c946c(const void *source);

/* Resets the figure slots and the per-figure stroke stages, then builds the
 * palette state and the three palette buffers: buffer 0 reloads from buffer
 * 2's colours and buffer 1 from buffer 0's. */
void func_ov007_020b69c4(void) {
    int i;

    WORK->model = 0;
    WORK->figure = 0;
    WORK->altFigure = 0;
    WORK->figureIndex = WORK->nextFigureIndex = 0;

    for (i = 0; i < 2; i++) {
        WORK->strokeStage[i] = 0;
    }

    WORK->paletteState = (Ov007MenuState *)func_ov007_020c9388(0);

    for (i = 0; i < 3; i++) {
        WORK->paletteBufs[i] = func_ov007_020c946c(0);
    }

    WORK->paletteBufs[0]->source = WORK->paletteBufs[2]->colors;
    WORK->paletteBufs[1]->source = WORK->paletteBufs[0]->colors;
}
