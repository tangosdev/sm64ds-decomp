#include "Ov007OamNode.h"

/* Starts animation animIndex on the node in `mode` (0 = stopped, 1 =
 * looping, else once), backwards when rev is 1. A frame >= 0 also restarts
 * it at that frame; a negative one keeps the current frame and tick. */
void Sprite_SetAnimation(Ov007OamNode *node, int animIndex, int mode, int rev, int frame) {
    node->animIndex = animIndex;
    if (frame >= 0) {
        node->frame = frame;
        node->tick = 0;
    }
    node->mode = mode;
    node->rev = rev;
}
