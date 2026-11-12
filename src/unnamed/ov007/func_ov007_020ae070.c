#include "types.h"
#include "ds_width.h"
#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "Ov007OamNode.h"
#include "Ov007Item.h"
extern void Sprite_SetAnimation(Ov007OamNode *node, int animIndex, int mode, int rev, int frame);

#ifndef SM64DS_HOST_64
typedef char
    Ov007Work_selectedSlot_must_be_at_0x58[DS_IADDR(&((Ov007Work *)0)->selectedSlot) == 0x58 ? 1 : -1];
#endif

/* func_ov007_020add3c calls this while the item is in state 3. On that
 * state's first tick, item 1 sets its node's animation to follow the mode
 * states[2] is in. */
void func_ov007_020ae070(Ov007ItemNode *icon) {
    Ov007Item *item = icon->item;
    u16 id = item->id;
    int ticks = item->state->stateTicks;
    Ov007Work *g = WORK;
    short mode = g->states[2]->currentState;

    if (id != 1)
        return;
    if (ticks != 0)
        return;

    switch (mode) {
    case 3:
        Sprite_SetAnimation(icon->node, 0, 0, 0, 0);
        break;
    case 14:
        Sprite_SetAnimation(icon->node, 3, 0, 0, 0);
        break;
    case 4:
        Sprite_SetAnimation(icon->node, 2, 0, 0, 0);
        break;
    case 5:
        Sprite_SetAnimation(icon->node, 1, 0, 0, 0);
        break;
    case 7:
        if (g->transitionMode == 1)
            Sprite_SetAnimation(icon->node, 1, 0, 0, 0);
        else
            Sprite_SetAnimation(icon->node, 2, 0, 0, 0);
        break;
    case 8:
        Sprite_SetAnimation(icon->node, 4, 0, 0, 0);
        break;
    case 9:
        Sprite_SetAnimation(icon->node, 5, 0, 0, 0);
        break;
    case 10:
        Sprite_SetAnimation(icon->node, 6, 0, 0, 0);
        break;
    case 12:
        if (g->selectedSlot == -1)
            Sprite_SetAnimation(icon->node, 7, 0, 0, 0);
        else
            Sprite_SetAnimation(icon->node, 8, 0, 0, 0);
        break;
    case 13:
        Sprite_SetAnimation(icon->node, 9, 0, 0, 0);
        break;
    }
}
