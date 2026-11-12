#include "Ov007OamNode.h"
#include "nitro/gx/gxcommon.h"

extern int func_ov007_020c0b78(GXOamAttr *oam, Ov007OamNode *node, int x, int y, int shownIn,
                               unsigned char priorityIn, int mosaicIn, int slot);

/* Draws the list's nodes (the head, its siblings and their subtrees) into
 * OAM buffer `oam` from slot `slot` on. Returns the next free slot, or 0
 * for an empty list. */
int func_ov007_020c0b20(Ov007OamList *list, void *oam, int slot) {
    Ov007OamNode *head = list->head;
    if (head == 0)
        return 0;
    return func_ov007_020c0b78(oam, head, 0, 0, 1, 4, 0, slot);
}
