#include "Ov007OamNode.h"
#include "Ov007Item.h"
#include "Ov007MenuMem.h"

extern void func_ov007_020c1180(Ov007OamNode *node);
extern void func_ov007_020aea4c(Ov007Item *item);

/* Frees an item node: for item 0 the child sprite func_ov007_020ade58 gave
 * it, then the item, the sprite node and the record. */
void func_ov007_020ade14(Ov007ItemNode *in) {
    if (in->item->id == 0)
        func_ov007_020c1180(in->node->head);
    func_ov007_020aea4c(in->item);
    func_ov007_020c1180(in->node);
    func_ov007_020c3d1c(in);
}
