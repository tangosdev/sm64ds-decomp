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

    sMenuWork->model = 0;
    sMenuWork->figure = 0;
    sMenuWork->altFigure = 0;
    sMenuWork->figureIndex = sMenuWork->nextFigureIndex = 0;

    for (i = 0; i < 2; i++) {
        sMenuWork->strokeStage[i] = 0;
    }

    sMenuWork->paletteState = (Ov007MenuState *)func_ov007_020c9388(0);

    for (i = 0; i < 3; i++) {
        sMenuWork->paletteBufs[i] = func_ov007_020c946c(0);
    }

    sMenuWork->paletteBufs[0]->source = sMenuWork->paletteBufs[2]->colors;
    sMenuWork->paletteBufs[1]->source = sMenuWork->paletteBufs[0]->colors;
}
