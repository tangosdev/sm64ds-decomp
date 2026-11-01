/* func_ov007_020c0b78 at 0x020c0b78
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov007).
 */
#include "Ov007OamNode.h"

struct E8 {
    int a, b;
};

extern void func_ov007_020c0eb0(struct E8 *, unsigned short *, int, int, int, int, int, int);
extern void func_ov007_020c0d70(Ov007OamNode *node, Ov007CellAnim *anim);
extern int func_ov007_020c0b78(struct E8 *oam, Ov007OamNode *node, int x, int y, int shownIn,
                               unsigned char priorityIn, int mosaicIn, int slot);

/* Draws `node` and its subtree into OAM buffer `oam` from `slot` on: the
 * node at (x, y) plus its own offset, then its next sibling at the parent's
 * position and its children at its own. A node shows only while it and every
 * ancestor are visible (shownIn), takes the parent's priority unless that is
 * > 3, and is mosaic when it or an ancestor is. Returns the next free
 * slot. */
int func_ov007_020c0b78(struct E8 *oam, Ov007OamNode *node, int x, int y, int shownIn,
                        unsigned char priorityIn, int mosaicIn, int slot) {
    int shown, mosaic, screenX, screenY;
    unsigned char priority = priorityIn;

    screenX = node->x + x;
    screenY = node->y + y;

    if (priority > 3)
        priority = node->priority;

    if (node->mosaic != 0 || mosaicIn != 0)
        mosaic = 1;
    else
        mosaic = 0;

    if (node->visible != 0 && shownIn != 0)
        shown = 1;
    else
        shown = 0;

    if (shown) {
        if (node->animCount) {
            Ov007CellAnim *anim = node->anims[node->animIndex];
            if (anim) {
                int i;
                Ov007Cell *cell = anim->frames[node->frame];
                for (i = 0; i < cell->count; i++) {
                    func_ov007_020c0eb0(&oam[slot], &cell->oam[i].attr0,
                                        (screenX >> 12) + anim->offsetX,
                                        (screenY >> 12) + anim->offsetY, priority, node->flip,
                                        mosaic, node->affineIndex);
                    slot++;
                }
                func_ov007_020c0d70(node, anim);
            }
        }
    }

    if (node->next)
        slot = func_ov007_020c0b78(oam, node->next, x, y, shownIn, priorityIn, mosaicIn, slot);
    if (node->head)
        slot =
            func_ov007_020c0b78(oam, node->head, screenX, screenY, shown, priority, mosaic, slot);

    return slot;
}
