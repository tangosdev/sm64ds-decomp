#include "Ov007MenuState.h"
#include "Ov007Tween.h"
#include "Ov007Work.h"
#include "Ov007Item.h"
#include "Ov007LayerSlot.h"
extern void func_ov007_020c0638(struct Ov007BgLayer *layer, int x, int y, u16 tile);

/* Resets the slot choice: requests state 5 on slot items 14..16, clears
 * selectedSlot/targetSlot, parks both tweens with their outputs at
 * 0x20000/0x30000, and scrolls layers 6 and 7 back to the origin. */
void func_ov007_020b0244(void) {
    sMenuWork->itemViews[14]->item->state->requestedState = 5;
    sMenuWork->itemViews[15]->item->state->requestedState = 5;
    sMenuWork->itemViews[16]->item->state->requestedState = 5;
    sMenuWork->targetSlot = -1;
    sMenuWork->selectedSlot = sMenuWork->targetSlot;
    sMenuWork->recordBoardAlt = 0;
    sMenuWork->tweenOut[0] = 0x20000;
    sMenuWork->tweenOut[1] = 0x30000;
    sMenuWork->tweens[0]->state = 0;
    sMenuWork->tweens[1]->state = 0;
    func_ov007_020c0638(sMenuWork->layerSlots[6]->layer, 0, 0, 0);
    func_ov007_020c0638(sMenuWork->layerSlots[7]->layer, 0, 0, 0);
}
