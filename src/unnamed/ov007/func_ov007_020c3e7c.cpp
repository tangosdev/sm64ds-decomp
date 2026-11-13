//cpp
#include "Ov007SpriteGroup.h"
#include "Ov007MenuMem.h"

extern void func_ov007_020c40b4(Ov007SpriteItem *item);
extern "C" void func_ov007_020c1180(Ov007OamNode *node);

/* Frees a sprite group: each item record and their table, the root node,
 * the extra record and the group itself. */
void func_ov007_020c3e7c(Ov007SpriteGroup *grp) {
    int i;
    for (i = 0; i < *grp->layout; i++) {
        func_ov007_020c40b4(grp->items[i]);
    }
    func_ov007_020c3d1c(grp->items);
    func_ov007_020c1180(grp->root);
    func_ov007_020c40b4(grp->extra);
    func_ov007_020c3d1c(grp);
}
