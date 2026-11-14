#include "Ov007Work.h"
#include "Ov007BgLayer.h"
#include "Ov007LayerSlot.h"

extern void func_ov007_020c02d8(void *screen, int rows, unsigned short tile);
extern void func_ov007_020c1d78(int i);

/* Blanks layer slot 5's screen to tile 0x300 (0x18 rows) and marks it for
 * upload. */
void func_ov007_020adb84(void) {
    Ov007LayerSlot *slot = WORK->layerSlots[5];
    func_ov007_020c02d8(slot->layer->screen, 0x18, 0x300);
    slot->layer->dirty = 1;
    func_ov007_020c1d78(0);
}
