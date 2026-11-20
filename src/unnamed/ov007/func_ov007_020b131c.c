#include "Ov007Work.h"
#include "Ov007BgLayer.h"
#include "Ov007LayerSlot.h"

extern void func_ov007_020c0638(Ov007BgLayer *layer, int x, int y, u16 tile);

/* Resets the scroll of layer slots 6 and 1 and rebuilds both screens from
 * their sources at a y offset of -0xc0 pixels (func_ov007_020c0638). */
void func_ov007_020b131c(void) {
    Ov007Work *work = gMenuWork;
    Ov007BgLayer *a = work->layerSlots[6]->layer;
    Ov007BgLayer *b = work->layerSlots[1]->layer;
    a->scrollX = 0;
    a->scrollY = 0;
    a->fineScrollY = 0;
    a->fineScrollX = a->fineScrollY;
    b->scrollX = 0;
    b->scrollY = 0;
    b->fineScrollY = 0;
    b->fineScrollX = b->fineScrollY;
    func_ov007_020c0638(a, 0, -192, 0);
    func_ov007_020c0638(b, 0, -192, 0);
}
