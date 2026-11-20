#include "types.h"
#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "Ov007OamNode.h"
#include "Ov007Item.h"

extern void Sprite_SetAnimation(Ov007OamNode *node, int animIndex, int mode, int rev, int frame);

/* Picks the next state for one item node from the menu state and the item's
 * own state. */
int func_ov007_020ae558(Ov007ItemNode *self) {
    Ov007Work *g = gMenuWork;
    Ov007MenuState *mode = g->states[2];
    int modeTicks = mode->stateTicks;
    Ov007Item *item = self->item;
    int modeState = mode->currentState;
    Ov007ItemLayout *layout = item->layout;
    int entering = (modeTicks == 0);
    Ov007MenuState *itemState = item->state;
    int kind = layout->kind;
    int id = item->id;
    int itemCurrent = itemState->currentState;
    int ret = itemState->requestedState;

    switch (modeState) {
    case 0:
        if (entering)
            ret = 1;
        break;
    case 1:
        if (entering) {
            if (id == 1)
                Sprite_SetAnimation(self->node, 0, 0, 0, 0);
        }
        break;
    case 3:
        if (entering) {
            if (kind == 1)
                ret = 7;
            else if (id == 7)
                ret = 1;
        }
        break;
    case 2:
        if (entering) {
            if (kind == 1) {
                if (id != 2)
                    ret = 4;
            }
        }
        break;
    case 4:
        if (entering) {
            if (kind == 1)
                ret = 7;
        }
        break;
    case 5:
        if (entering) {
            if (kind == 1)
                ret = 7;
        }
        break;
    case 7:
        if (entering) {
            if (kind == 1)
                ret = 3;
        }
        break;
    case 6:
        if (entering) {
            if (kind == 1)
                ret = 4;
        }
        break;
    case 8:
        if (entering) {
            short d = mode->previousState;
            int t = (d == 1 || d == 3);
            if (kind == 1) {
                if (t) {
                    if (id != 2) {
                        ret = 0;
                        if (id == 1)
                            Sprite_SetAnimation(self->node, 4, 0, 0, 0);
                    } else
                        ret = 1;
                } else
                    ret = 7;
            } else if (kind == 5) {
                ret = 0;
                Sprite_SetAnimation(self->node, 0, 1, 0, 0);
            } else
                ret = 1;
        }
        break;
    case 9:
    case 10:
    case 13:
        if (entering) {
            if (kind == 1)
                ret = 7;
        }
        break;
    case 11:
        if (kind == 1) {
            if (entering)
                ret = 4;
        }
        break;
    case 12:
        if (kind == 1) {
            if (entering)
                ret = 7;
            else {
                /* the ROM reloads the work pointer here */
                Ov007Work *g2 = gMenuWork;
                short menuState = g2->states[1]->currentState;
                u8 nodeAnim = g2->itemNodes[1]->node->animIndex;
                int x;
                if (menuState != 7)
                    break;
                if (itemCurrent != 0)
                    break;
                x = g->selectedSlot;
                if ((x != -1 && nodeAnim == 7) || (x == -1 && nodeAnim == 8))
                    ret = 7;
            }
        }
        break;
    case 14:
        if (entering) {
            if (kind == 1)
                ret = 7;
            else if (id == 7)
                ret = 0;
        }
        break;
    }
    return ret;
}
