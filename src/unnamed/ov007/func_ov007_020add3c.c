#include "types.h"
#include "Ov007MenuState.h"
#include "Ov007OamNode.h"
#include "Ov007Item.h"

extern int func_ov007_020ae558(Ov007ItemNode *self);
extern void func_ov007_020ae834(Ov007Item *item);
extern void func_ov007_020ae2d0(Ov007ItemNode *self);
extern void func_ov007_020ae070(Ov007ItemNode *self);
extern void func_ov007_020ae454(Ov007ItemNode *self);

/* Steps one item node a frame: func_ov007_020ae558 picks the item's next
 * state, func_ov007_020ae834 runs it, states 0 and 3 add their effects, and
 * the node follows the item's position and visibility, hidden once it is
 * at or above y = -0x64 (Q12). */
void func_ov007_020add3c(Ov007ItemNode *self) {
    int r = func_ov007_020ae558(self);
    self->item->state->requestedState = r;
    func_ov007_020ae834(self->item);
    switch (self->item->state->currentState) {
    case 0:
        func_ov007_020ae2d0(self);
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
        func_ov007_020ae070(self);
        break;
    case 4:
        break;
    case 5:
        break;
    case 6:
        break;
    case 7:
        break;
    }
    func_ov007_020ae454(self);
    self->node->x = self->item->posX;
    self->node->y = self->item->posY;
    self->node->visible = self->item->visible;
    if (self->node->visible != 0) {
        if (self->node->y <= -0x64000)
            self->node->visible = 0;
    }
}
