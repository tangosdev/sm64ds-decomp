#include "Ov007MenuState.h"
#include "Ov007Tween.h"
#include "Ov007Work.h"
extern void func_ov007_020c0638(int a, int b, int c, int d);
extern char* data_ov007_0210342c;

/* The 0x2c-byte record func_ov007_020aea6c allocates; names as in
 * func_ov007_020ae070, only the slot this file touches. */
typedef struct Ov007Item {
    u16 id; /* 0x00: aea6c's first argument */
    char pad_02[2];
    Ov007MenuState *state; /* 0x04: func_ov007_020c9388(1) */
} Ov007Item;

/* The 0x1c record func_ov007_020b4c04 returns; names as in
 * func_ov007_020b9880, only the slot read here. */
typedef struct Ov007ItemView {
    Ov007Item *item; /* 0x00 */
} Ov007ItemView;

/* The 0xc-byte record func_ov007_020adc08 returns; names as there, only the
 * layer read here. */
typedef struct Ov007LayerSlot {
    int state; /* 0x00: an Ov007MenuState (func_ov007_020c9388) */
    int layer; /* 0x04: the Ov007BgLayer func_ov007_020c0638 scrolls */
} Ov007LayerSlot;

/* The extern keeps the tree's char * spelling; the typed view is reached
 * through a cast macro. */
#define G ((char*)data_ov007_0210342c)
#define WORK ((Ov007Work *)data_ov007_0210342c)

/* Resets the slot choice: requests state 5 on slot items 14..16, clears
 * selectedSlot/targetSlot, parks both tweens with their outputs at
 * 0x20000/0x30000, and scrolls layers 6 and 7 back to the origin. */
void func_ov007_020b0244(void)
{
    WORK->itemViews[14]->item->state->requestedState = 5;
    WORK->itemViews[15]->item->state->requestedState = 5;
    WORK->itemViews[16]->item->state->requestedState = 5;
    WORK->targetSlot = -1;
    WORK->selectedSlot = WORK->targetSlot;
    *(int*)(G + 0x60) = 0;
    WORK->tweenOut[0] = 0x20000;
    WORK->tweenOut[1] = 0x30000;
    WORK->tweens[0]->state = 0;
    WORK->tweens[1]->state = 0;
    func_ov007_020c0638(WORK->layerSlots[6]->layer, 0, 0, 0);
    func_ov007_020c0638(WORK->layerSlots[7]->layer, 0, 0, 0);
}
